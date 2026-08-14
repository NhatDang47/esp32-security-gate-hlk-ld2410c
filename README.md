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
}

body {
  background-color: var(--ctp-base);
  color: var(--ctp-text);
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
  line-height: 1.6;
}
.gradient-text {
  background: linear-gradient(90deg, var(--ctp-sky), var(--ctp-sapphire), var(--ctp-blue));
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  font-weight: 800;
  border-bottom: none;
}
h2, h3, h4 { 
  color: var(--ctp-sapphire); 
  border-bottom: 2px solid var(--ctp-surface1); 
  padding-bottom: 5px; 
  margin-top: 25px;
}
a { color: var(--ctp-mauve); text-decoration: none; font-weight: 600; transition: all 0.3s ease; }
a:hover { color: var(--ctp-pink); text-decoration: underline; }
code { 
  background-color: var(--ctp-surface0); 
  color: var(--ctp-green); 
  padding: 3px 6px; 
  border-radius: 6px; 
  font-family: 'Consolas', monospace;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}
pre { 
  background-color: var(--ctp-mantle); 
  border: 1px solid var(--ctp-surface1); 
  border-left: 4px solid var(--ctp-sapphire); 
  padding: 15px; 
  border-radius: 8px; 
  box-shadow: inset 0 2px 10px rgba(0,0,0,0.2);
}
blockquote { 
  border-left: 4px solid var(--ctp-teal); 
  background: linear-gradient(90deg, var(--ctp-surface0), transparent);
  padding: 15px 20px; 
  margin: 20px 0; 
  border-radius: 0 8px 8px 0; 
  color: var(--ctp-subtext1); 
  font-style: italic;
}
.hero-img {
  border-radius: 12px;
  box-shadow: 0 8px 25px rgba(0,0,0,0.4);
  transition: transform 0.3s;
}
.hero-img:hover {
  transform: scale(1.02);
}
.branch-btn {
  display: inline-block;
  margin: 10px 0;
  transition: transform 0.2s;
}
.branch-btn:hover {
  transform: translateY(-2px);
}
</style>

<div align="center">
  <img src="../picture/Hình 13 Code và giao diện Node-red.png" width="650" alt="Node-RED UI" class="hero-img" />
  <h1 class="gradient-text">Node-RED Dashboard & Flow</h1>
  <p><i>Giao diện điều khiển trung tâm và giám sát trực quan cho hệ thống cửa an ninh</i></p>
  
  <p>
    <a href="https://github.com/NhatDang47/esp32-security-gate-hlk-ld2410c/tree/main" class="branch-btn">
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
  <img src="../picture/Hình 17 Biểu đồ sự kiện vàora.png" alt="Biểu đồ sự kiện" width="650" class="hero-img" />
  <br/>
  <i>Đồ thị biểu diễn thông số sự kiện vào/ra thu nhận từ Radar</i>
</p>

<blockquote>
<b>Tích hợp liên kết:</b> Mã nguồn chương trình nhúng chi tiết được lưu trữ tại nhánh <b>main</b>. Vui lòng chuyển nhánh để truy xuất thông tin nền tảng phần cứng ESP32.
</blockquote>
