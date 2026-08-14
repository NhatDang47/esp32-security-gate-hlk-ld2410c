
<div align="center">
  <img src="picture/Hình 13 Code và giao diện Node-red.png" width="650" alt="Node-RED UI" />
  <h1>Node-RED Dashboard & Flow</h1>
  <p><i>Giao diện điều khiển trung tâm và giám sát trực quan cho hệ thống cửa an ninh</i></p>
  
  <p>
    <a href="https://github.com/NhatDang47/esp32-security-gate-hlk-ld2410c/tree/main">
      <img src="https://img.shields.io/badge/Branch-Main_(ESP32)-00599C?style=for-the-badge&logo=espressif&logoColor=white" alt="Main Branch" />
    </a>
  </p>

  <!-- Badges -->
  <p>
    <img src="https://img.shields.io/badge/Platform-Node--RED-8F0000?style=flat-square&logo=nodered&logoColor=white" alt="Node-RED" />
    <img src="https://img.shields.io/badge/Protocol-MQTT-660066?style=flat-square&logo=mqtt&logoColor=white" alt="MQTT" />
    <img src="https://img.shields.io/badge/Language-JavaScript-F7DF1E?style=flat-square&logo=javascript&logoColor=black" alt="JavaScript" />
    <img src="https://img.shields.io/badge/Format-JSON-000000?style=flat-square&logo=json&logoColor=white" alt="JSON" />
  </p>
</div>

---

## 📖 Tổng quan kiến trúc
Tài liệu này thuộc nhánh `node-red`, cung cấp tệp cấu hình **`flows.json`** phục vụ việc khởi tạo máy chủ điều khiển cục bộ (On-premises) hoặc điện toán đám mây (Cloud).

Trong cấu trúc hệ thống, vi điều khiển ESP32 (Edge device) xử lý tín hiệu phần cứng tại hiện trường. Tầng ứng dụng Node-RED đóng vai trò trung tâm phân tích dữ liệu thông qua giao thức **MQTT** với các nhiệm vụ:
- Đồng bộ và lưu trữ nhật ký dữ liệu từ Node.
- Kết xuất đồ thị phân tích thông số khoảng cách và mức năng lượng tín hiệu.
- Giao diện hóa trạng thái hoạt động: Nhận diện vào/ra, cảnh báo an toàn.
- Cấu trúc lệnh điều khiển từ xa để đồng bộ hóa hoặc reset trạng thái vi điều khiển ESP32.

---

## 🚀 Cài đặt & Tích hợp Flow

1. **Khởi tạo môi trường Node-RED:**
   Sử dụng trình quản lý gói NodeJS để cài đặt hệ sinh thái:
   ```bash
   npm install -g --unsafe-perm node-red
   ```
   Thông qua *Manage Palette*, tiến hành cài đặt thành phần mở rộng: `node-red-dashboard`.

2. **Định tuyến MQTT Broker:**
   Triển khai Broker độc lập (VD: Eclipse Mosquitto). Đảm bảo tính liên kết đồng bộ IP và Port giao tiếp giữa thiết bị phần cứng ESP32 và máy chủ Node-RED.

3. **Cập nhật dữ liệu Flow:**
   - Khởi chạy giao diện Node-RED trên trình duyệt.
   - Chọn thuộc tính **Import** từ thanh công cụ.
   - Trích xuất toàn bộ cấu trúc dữ liệu JSON từ file `flows.json` vào hộp thoại.
   - Kích hoạt tiến trình **Deploy**. Hệ thống Dashboard sẽ khả dụng tại: `http://<node-red-ip>:1880/ui`.

---

## 📊 Mô hình giám sát thực tế

Giao diện cung cấp giải pháp phân tích chi tiết dữ liệu không gian và phản xạ từ cảm biến theo miền thời gian, hỗ trợ người dùng hiệu chỉnh hệ số thuật toán một cách linh hoạt.

<p align="center">
  <img src="picture/Hình 17 Biểu đồ sự kiện vàora.png" alt="Biểu đồ sự kiện" width="650" />
  <br/>
  <i>Đồ thị biểu diễn thông số sự kiện vào/ra thu nhận từ Radar</i>
</p>

