Dưới đây là nội dung dạng văn bản (text only) cho file `README.md` của hệ thống:

---

# Gas Leak Detection and Warning System

## Mô tả dự án

Hệ thống phát hiện và cảnh báo rò rỉ khí gas sử dụng vi điều khiển STM32F401CEU6 kết hợp với cảm biến MQ-5 và DHT11. Khi phát hiện khí vượt ngưỡng, hệ thống cảnh báo bằng còi, LED, LCD và tự động ngắt nguồn gas. Dữ liệu được gửi về Blynk và ghi log lên Google Sheets qua ESP32, giúp người dùng giám sát từ xa và theo dõi lịch sử rò rỉ.

## Phần cứng sử dụng

* STM32F401CEU6: vi điều khiển chính
* MQ-5: cảm biến khí gas LPG, CH4, CO
* DHT11: cảm biến nhiệt độ và độ ẩm
* LCD1602 (I2C): hiển thị nồng độ khí và trạng thái
* ESP32: truyền dữ liệu lên Blynk và Google Sheets
* Buzzer: cảnh báo âm thanh
* LED (Xanh lá, xanh dương, vàng, đỏ): cảnh báo trực quan
* Relay: ngắt van cấp gas
* SW1, SW2: nút điều khiển Start/Stop và Reset

## Chức năng chính

* Đọc và hiệu chỉnh nồng độ khí gas theo nhiệt độ/độ ẩm
* Hiển thị thông tin trạng thái trên LCD
* Cảnh báo bằng LED, buzzer, và relay
* Gửi cảnh báo thời gian thực lên ứng dụng Blynk
* Ghi log cảnh báo lên Google Sheets (thời gian, nồng độ, trạng thái)


## Ngưỡng cảnh báo khí gas

* < 300 ppm: An toàn (LED xanh dương)
* 300–500 ppm: Cảnh báo nhẹ (LED vàng)
* 500–1000 ppm: Nguy hiểm cao (LED đỏ nháy 1Hz)
* > 1000 ppm: Cảnh báo nguy hiểm (LED đỏ nháy nhanh, bật buzzer & relay)

## Giao tiếp từ xa

ESP32 nhận dữ liệu từ STM32 qua UART, sau đó:

* Gửi lên Blynk
* Ghi log lên Google Sheets theo định dạng:

  * Thời gian: 2025-06-18 10:12:05
  * Nồng độ: 1150 ppm
  * Trạng thái: Nguy hiểm


