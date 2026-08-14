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
blockquote { border-left: 4px solid var(--ctp-teal); background-color: var(--ctp-surface0); padding: 10px 15px; margin: 15px 0; border-radius: 0 8px 8px 0; color: var(--ctp-subtext1); }
</style>

<div align="center">
  <img src="../picture/Hình 13 Code và giao diện Node-red.png" width="600" alt="Node-RED UI" />
  <h1>Node-RED Dashboard & Flow</h1>
  <p><i>Giao diện điều khiển trung tâm và giám sát trực quan cho Cổng An Ninh ESP32</i></p>
  
  <p>
    <a href="https://github.com/NhatDang47/esp32-security-gate-hlk-ld2410c/tree/main"><b>👉 Chuyển về nhánh Main (Mã nguồn ESP32)</b></a>
  </p>
</div>

---

## 📖 Giới thiệu Node-RED
Nhánh `node-red` này chứa tệp cấu hình **`flows.json`** để khởi tạo máy chủ điều khiển cục bộ (hoặc trên Cloud).
Trong khi ESP32 làm nhiệm vụ lấy mẫu phần cứng và xử lý Radar tại hiện trường, thì hệ thống Node-RED sẽ kết nối thông qua giao thức **MQTT** để:
- Thu thập nhật ký dữ liệu từ Node.
- Vẽ đồ thị khoảng cách và năng lượng.
- Hiển thị trực quan trạng thái Vào/Ra, Đứng Im theo thời gian thực.
- Xuất lệnh cấu hình / Reset từ xa xuống phần cứng ESP32.

---

## 🚀 Hướng dẫn Cài đặt & Khôi phục Flow

1. **Cài đặt Node-RED & Node-RED Dashboard:**
   Nếu bạn chưa có Node-RED, hãy chạy lệnh cài đặt trên NodeJS:
   ```bash
   npm install -g --unsafe-perm node-red
   ```
   Sau đó vào giao diện Quản lý Palette (Manage Palette) cài đặt thêm package: `node-red-dashboard`.

2. **Cấu hình MQTT Broker:**
   Hệ thống sử dụng Broker riêng biệt (Ví dụ Mosquitto). Chắc chắn rằng ESP32 và Node-RED đều cùng chỏ về chung một IP của Broker.

3. **Khôi phục (Import) Flow:**
   - Mở giao diện Node-RED trên trình duyệt.
   - Nhấn vào góc phải trên cùng (Menu) -> Chọn **Import**.
   - Copy toàn bộ nội dung file `flows.json` trong nhánh này và paste vào ô Import.
   - Bấm **Deploy**. Giao diện UI sẽ có tại đường dẫn `http://<node-red-ip>:1880/ui`.

---

## 📊 Biểu đồ Giám Sát Thực Tế

Hệ thống biểu diễn chi tiết thời điểm xuất hiện sự kiện và khoảng cách cũng như lực năng lượng phản xạ để tinh chỉnh thuật toán.

<p align="center">
  <img src="../picture/Hình 17 Biểu đồ sự kiện vàora.png" alt="Biểu đồ sự kiện" width="700"/>
  <br/>
  <i>Biểu đồ thể hiện sự kiện Vào/Ra từ Radar</i>
</p>

<blockquote>
Đừng quên ghé qua nhánh <b>main</b> để tham khảo phần Code nhúng cho hệ thống cửa thông minh dưới thiết bị Edge ESP32.
</blockquote>
