<style>
:root {
  --ctp-rosewater: #f5e0dc;
  --ctp-flamingo: #f2cdcd;
  --ctp-pink: #f5c2e7;
  --ctp-mauve: #cba6f7;
  --ctp-red: #f38ba8;
  --ctp-maroon: #eba0ac;
  --ctp-peach: #fab387;
  --ctp-yellow: #f9e2af;
  --ctp-green: #a6e3a1;
  --ctp-teal: #94e2d5;
  --ctp-sky: #89dceb;
  --ctp-sapphire: #74c7ec;
  --ctp-blue: #89b4fa;
  --ctp-lavender: #b4befe;
  --ctp-text: #cdd6f4;
  --ctp-subtext1: #bac2de;
  --ctp-subtext0: #a6adc8;
  --ctp-overlay2: #9399b2;
  --ctp-overlay1: #7f849c;
  --ctp-overlay0: #6c7086;
  --ctp-surface2: #585b70;
  --ctp-surface1: #45475a;
  --ctp-surface0: #313244;
  --ctp-base: #1e1e2e;
  --ctp-mantle: #181825;
  --ctp-crust: #11111b;
}

body {
  background-color: var(--ctp-base);
  color: var(--ctp-text);
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
}
h1, h2, h3, h4 { color: var(--ctp-mauve); border-bottom: 2px solid var(--ctp-surface1); padding-bottom: 5px; }
h1 { color: var(--ctp-pink); }
a { color: var(--ctp-blue); text-decoration: none; font-weight: bold; }
a:hover { color: var(--ctp-sky); text-decoration: underline; }
code { background-color: var(--ctp-surface0); color: var(--ctp-peach); padding: 2px 6px; border-radius: 4px; }
pre { background-color: var(--ctp-mantle); border: 1px solid var(--ctp-surface1); border-left: 4px solid var(--ctp-mauve); padding: 15px; border-radius: 8px; }
table { border-collapse: collapse; width: 100%; margin: 15px 0; background-color: var(--ctp-base); }
th, td { border: 1px solid var(--ctp-surface1); padding: 12px; text-align: center; }
th { background-color: var(--ctp-surface0); color: var(--ctp-sapphire); font-weight: bold; }
tr:nth-child(even) { background-color: var(--ctp-mantle); }
blockquote { border-left: 4px solid var(--ctp-green); background-color: var(--ctp-surface0); padding: 10px 15px; margin: 15px 0; border-radius: 0 8px 8px 0; color: var(--ctp-subtext1); }
</style>

<div align="center">
  <img src="picture/Hình 1 cảm biến Radar HLK-LD2410C.png" width="300" alt="HLK-LD2410C" />
  <h1>ESP32 Security Gate & People Counter</h1>
  <p><i>Hệ thống Cửa An Ninh Thông Minh & Đếm Người sử dụng Cảm biến Radar HLK-LD2410C</i></p>
  
  <p>
    <a href="https://github.com/NhatDang47/esp32-security-gate-hlk-ld2410c/tree/node-red"><b>👉 Chuyển sang nhánh Node-RED Dashboard</b></a>
  </p>
</div>

---

## 📖 Giới thiệu Dự án
Nhánh `main` này chứa toàn bộ Source Code nhúng cho Vi điều khiển **ESP32** viết bằng C/C++ (ESP-IDF / FreeRTOS).
Hệ thống sử dụng cảm biến **Radar HLK-LD2410C** hoạt động ở dải tần 24GHz để phát hiện chính xác trạng thái tĩnh và động của mục tiêu, phục vụ cho ứng dụng chống kẹp cửa tự động và đếm số lượng người ra vào.

### 🌟 Tính năng chính
- Phát hiện người **đi vào/đi ra** với độ chính xác cao.
- **Chống kẹp:** Cảnh báo và mở cửa khẩn cấp khi có người đứng im trong vùng cửa quá thời gian quy định (10s).
- **Web Server Cấu Hình:** Tích hợp Access Point và Web Server cục bộ tại IP `192.168.4.1` để người dùng điền WiFi & MQTT Broker.
- **Hiển thị trực quan:** Màn hình LCD 20x4 qua I2C.
- **Giao tiếp IoT:** Gửi luồng dữ liệu theo thời gian thực (Telemetry) qua giao thức MQTT đến Server/Node-RED.

---

## 🧱 Sơ đồ Khối & Nguyên lý

<p align="center">
  <img src="picture/Hình 5 Sơ đồ khối hệ thống.png" alt="Sơ đồ khối" width="600"/>
  <br/>
  <i>Sơ đồ khối hệ thống</i>
</p>
<p align="center">
  <img src="picture/Hình 6 sơ đồ nguyên lý.png" alt="Sơ đồ nguyên lý" width="600"/>
  <br/>
  <i>Sơ đồ nguyên lý kết nối phần cứng</i>
</p>

---

## 📡 Bảng Cấu Trúc Khung Truyền Dữ Liệu (Frame) HLK-LD2410C

Cảm biến giao tiếp qua UART. Hệ thống bóc tách các frame này để lấy thông tin trạng thái mục tiêu. Dưới đây là bảng biểu diễn Frame truyền của cảm biến:

| Byte Index | Giá trị Hex / Cấu trúc | Ý nghĩa |
| :---: | :---: | :--- |
| `0 - 3` | `F4 F3 F2 F1` | **Header:** Cờ báo bắt đầu chuỗi khung truyền |
| `4 - 5` | `XX XX` | **Data Length:** Độ dài dữ liệu (Intra-frame) |
| `6` | `02` | **Data Type:** Loại dữ liệu (0x02 = Engineering mode / Trạng thái) |
| `7` | `AA` | **Head:** Byte đầu của mảng dữ liệu |
| `8` | `00` / `01` / `02` / `03` | **Target Status:** 0=Không có ai, 1=Đang di chuyển, 2=Đứng im, 3=Cả hai |
| `9 - 10` | `XX XX` | **Moving Distance:** Khoảng cách mục tiêu đang di chuyển (cm) |
| `11` | `XX` (0-100) | **Moving Energy:** Năng lượng mục tiêu động (Độ phản xạ) |
| `12 - 13` | `XX XX` | **Static Distance:** Khoảng cách mục tiêu đứng im (cm) |
| `14` | `XX` (0-100) | **Static Energy:** Năng lượng mục tiêu tĩnh |
| `...` | `...` | Các byte thông tin mở rộng khác |
| `End-4 - End`| `F8 F7 F6 F5` | **Footer:** Cờ báo kết thúc khung truyền |

---

## ⚙️ Cài đặt & Sử dụng
1. Clone dự án: `git clone https://github.com/NhatDang47/esp32-security-gate-hlk-ld2410c.git`
2. Biên dịch & Nạp code thông qua ESP-IDF hoặc PlatformIO.
3. Khi khởi động lần đầu, kết nối vào WiFi AP **`ESP32_SecurityGate`** và truy cập **`http://192.168.4.1`** để điền SSID/Pass và MQTT Broker.
4. Triển khai Node-RED Dashboard *(Xem hướng dẫn bên nhánh `node-red`)*.

---

<p align="center">
  <img src="picture/Hình 12 từ thiết kế đến thi công.jpg" alt="Thực tế thi công" width="45%"/>
  <img src="picture/Hình 18 cảnh báo đứng imhú còi.jpg" alt="Cảnh báo hú còi" width="45%"/>
</p>

<blockquote>
Cảm ơn bạn đã quan tâm đến dự án! Đừng quên chuyển sang nhánh <b>Node-RED</b> để xem phần giao diện điều khiển Server nhé.
</blockquote>
