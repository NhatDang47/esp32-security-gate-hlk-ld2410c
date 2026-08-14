
<div align="center">
  <img src="picture/Hình 1 cảm biến Radar HLK-LD2410C.png" width="220" alt="HLK-LD2410C" />
  <h1>ESP32 Security Gate & People Counter</h1>
  <p><i>Hệ thống giám sát cửa an ninh và đếm người sử dụng cảm biến Radar HLK-LD2410C</i></p>
  
  <p>
    <a href="https://github.com/NhatDang47/esp32-security-gate-hlk-ld2410c/tree/node-red">
      <img src="https://img.shields.io/badge/Branch-Node--RED-8F0000?style=for-the-badge&logo=nodered&logoColor=white" alt="Node-RED Branch" />
    </a>
  </p>

  <!-- Badges -->
  <p>
    <img src="https://img.shields.io/badge/Espressif-ESP32-E7352C?style=flat-square&logo=espressif&logoColor=white" alt="ESP32" />
    <img src="https://img.shields.io/badge/Framework-ESP--IDF-E7352C?style=flat-square&logo=espressif&logoColor=white" alt="ESP-IDF" />
    <img src="https://img.shields.io/badge/OS-FreeRTOS-232A2F?style=flat-square&logo=freertos&logoColor=white" alt="FreeRTOS" />
    <img src="https://img.shields.io/badge/Language-C/C++-00599C?style=flat-square&logo=c%2B%2B&logoColor=white" alt="C/C++" />
    <img src="https://img.shields.io/badge/Protocol-MQTT-660066?style=flat-square&logo=mqtt&logoColor=white" alt="MQTT" />
    <img src="https://img.shields.io/badge/Interface-I2C%20%7C%20UART-4B0082?style=flat-square" alt="I2C/UART" />
  </p>
</div>

---

## 📖 Tổng quan hệ thống
Tài liệu này mô tả mã nguồn nhúng dành cho vi điều khiển **ESP32**, được phát triển trên nền tảng ESP-IDF kết hợp RTOS (FreeRTOS). Hệ thống tích hợp cảm biến **Radar HLK-LD2410C** (tần số 24GHz) nhằm phát hiện trạng thái tĩnh và động của mục tiêu.

Mục đích chính của dự án là phục vụ ứng dụng cảnh báo chống kẹp cửa tự động và thống kê lưu lượng người ra vào khu vực kiểm soát.

### 🌟 Đặc tả chức năng
- **Đo lường lưu lượng:** Xác định và đếm số lượng người đi vào/đi ra dựa trên dữ liệu biến thiên khoảng cách và năng lượng phản xạ.
- **Cơ chế an toàn (Chống kẹp):** Thiết lập cảnh báo và tác động tín hiệu ngoại vi khẩn cấp khi phát hiện mục tiêu tĩnh tại vùng cửa vượt quá thời gian thiết lập (10 giây).
- **Trang cấu hình cục bộ (Captive Web Portal):** Triển khai Access Point và Web Server cục bộ tại địa chỉ `192.168.4.1`, hỗ trợ cấu hình thông số mạng WiFi và địa chỉ MQTT Broker.
- **Hiển thị thông số:** Cập nhật dữ liệu thời gian thực lên màn hình LCD 20x4 thông qua chuẩn giao tiếp I2C.
- **Truyền thông IoT:** Đẩy chuỗi dữ liệu (Telemetry) bao gồm trạng thái, số lượng và năng lượng mục tiêu lên MQTT Broker để tích hợp với các nền tảng phân tích cấp cao.

---

## 🧱 Sơ đồ Khối & Nguyên lý

<p align="center">
  <img src="picture/Hình 5 Sơ đồ khối hệ thống.png" alt="Sơ đồ khối" width="550" />
  <br/>
  <i>Sơ đồ khối chức năng hệ thống</i>
</p>
<br/>
<p align="center">
  <img src="picture/Hình 6 sơ đồ nguyên lý.png" alt="Sơ đồ nguyên lý" width="550" />
  <br/>
  <i>Sơ đồ nguyên lý kết nối phần cứng</i>
</p>

---

## 📡 Cấu trúc khung truyền dữ liệu (Frame) HLK-LD2410C

Cảm biến giao tiếp với vi điều khiển qua chuẩn UART. Giao thức bóc tách các frame này để trích xuất thông tin định lượng. Cấu trúc một Data Frame được định nghĩa như sau:

| Byte Index | Giá trị Hex / Phân bổ | Diễn giải kỹ thuật |
| :---: | :---: | :--- |
| `0 - 3` | `F4 F3 F2 F1` | **Header:** Định dạng chuỗi bắt đầu khung truyền |
| `4 - 5` | `XX XX` | **Data Length:** Độ dài tải lượng dữ liệu (Intra-frame) |
| `6` | `02` | **Data Type:** Loại dữ liệu (0x02 = Engineering mode) |
| `7` | `AA` | **Head:** Byte khởi đầu mảng dữ liệu mục tiêu |
| `8` | `00` / `01` / `02` / `03` | **Target Status:** 0=Trống, 1=Động, 2=Tĩnh, 3=Tồn tại cả hai |
| `9 - 10` | `XX XX` | **Moving Distance:** Khoảng cách mục tiêu động (cm) |
| `11` | `XX` (0-100) | **Moving Energy:** Mức năng lượng mục tiêu động (%) |
| `12 - 13` | `XX XX` | **Static Distance:** Khoảng cách mục tiêu tĩnh (cm) |
| `14` | `XX` (0-100) | **Static Energy:** Mức năng lượng mục tiêu tĩnh (%) |
| `...` | `...` | Vùng dữ liệu mở rộng (Tuỳ cấu hình) |
| `End-4 - End`| `F8 F7 F6 F5` | **Footer:** Định dạng chuỗi kết thúc khung truyền |

---

## ⚙️ Cài đặt & Triển khai phần cứng
1. Sao chép kho lưu trữ: 
   ```bash
   git clone https://github.com/NhatDang47/esp32-security-gate-hlk-ld2410c.git
   ```
2. Biên dịch và nạp firmware thông qua công cụ ESP-IDF hoặc môi trường PlatformIO.
3. Cấp nguồn hệ thống. Thiết bị sẽ phát mạng WiFi Access Point **`ESP32_SecurityGate`**.
4. Truy cập **`http://192.168.4.1`** để điền thông số kết nối (SSID/Password) và thiết lập chuỗi định danh MQTT Broker.
5. Khởi tạo dịch vụ giám sát Node-RED *(Chi tiết tài liệu tham khảo nhánh `node-red`)*.

---

<p align="center">
  <img src="picture/Hình 12 từ thiết kế đến thi công.jpg" alt="Thực tế thi công" width="45%" class="hero-img" style="margin-right: 15px;" />
  <img src="picture/Hình 12 từ thiết kế đến thi công.png" alt="Mạch PCB" width="45%" />
</p>

