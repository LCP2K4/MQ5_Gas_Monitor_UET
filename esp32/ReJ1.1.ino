#define BLYNK_TEMPLATE_ID "TMPL6N_mfH9kQ"
#define BLYNK_TEMPLATE_NAME "Gas Alert"
#define BLYNK_AUTH_TOKEN "W1oeZ6AroaiI91XM63pObzGUwpZrK5sK"

#define RX 16
#define TX 17

#include <WiFi.h>
#include <WiFiMulti.h>
#include <BlynkSimpleEsp32.h>
#include <ESP_Google_Sheet_Client.h>
#include <WiFiUDP.h>
#include <NTPClient.h>
#include <HardwareSerial.h>

// ========== CẤU HÌNH WIFI ========== //
WiFiMulti wifiMulti;
HardwareSerial mySerial(1); // Sử dụng UART2 (RX=16, TX=17)

// ========== CẤU HÌNH GOOGLE SHEETS ========== //
#define PROJECT_ID "my-project-a-462210"
#define CLIENT_EMAIL "gas-alert@my-project-a-462210.iam.gserviceaccount.com"
const char PRIVATE_KEY[] = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQD1aQHY5hOKpnuV
dY7ulpQbm3LKWQthTBFG5kg7NSa+bPrjAQ+XarJgBMMZUVsW9wqVZPJf5yiUxEFS
iFA08qGYvqTwQZBZys4dEcV6ymafZNvZAtWyDj6n/h3gx2O2QyMuFmJ2Cb2ii/o0
YMpdD5gq4+ElHf+ZrSajGkVqbbyRB5+bZpwOfvxfqVBDRwizklDbL64aNQ3ODBQl
W7bCgbgjI4rOi8k9O8+EWYV5eGVKYeQD1bdEhvUdttGjbjRUINOjnnMxEjZm0kgp
ZLFoE2X0QtUVEm3pxqXS5+Xc9JEZzesbKM7uKxrsiio7287nmwG7ySD7VP4Ss6rN
pI/zIqOfAgMBAAECggEANDpWUc5ZT1MyDvNXaXBzcdKp4eWEdf8F2ERvD+a2aaVw
2Xni0xM6hhPNX6iATclin7u9l2pvKQYBbuqiwzn16Td9i8IwUePG7MnY8NJ9d6Oz
5CHlz8CXVhJPUoSstF/ib08FQmC9UogexiHpP1fvxwT7Z/ZT7OU8SU7wgf97t5Nw
lBwIGGBCccfvAGJIfM3m0+oWXhe4PyniOjkQvYpY0rmQiRkxkPKnlgOJUpHqmdnW
OIG616fOaiKGm2Pd39e/D5eOQdB8PmoOv1xnL/cMZvpFVpdaQS08JOtdndUCdrwy
2KQVfdpMhGlgpwJWHiW1yQCodulbIOyZ3aSXHCgbMQKBgQD8jTDHXDj/lULE5VUE
zCNMfN2drxPv7MwNzYz4r+CUzqKbv7y8j4hQyFge1WbXtr8iakhi7xxlg+VSxQER
w7QZkkHcxMmD1cBk1OuX3VgJ9SMu3eIC/jN1QMyu0mWdzM99nAu6B6kgfkzzVMrm
8jE05yemfZDuJcxG9aNYiCoNzwKBgQD4wtqKb4LrLb8sXgEflxtmiCuklQyuj8ym
betgUNmI69IQt5MZvo5Fe8WA/v4abwm2+bQIYnkIPxC9q6PH44zbtJrW/MtFS3Rp
h9zt41fhUtEyN6dW7nv+LO08eHaNQRHRW30cNOm6zPy5f0yAPyDXkGhxPUMaOfFe
m7X8WTDRMQKBgQDOxWZvgwEjuHf/LuhJ0j0ATadAh+xh3lU+49EMdyr/iqK1s+j5
1OY9hZooPH5EFNoueigj4DM2piNmfecfPUTCPu3vq6p1ZtdrdqwhHVcMaAjTQVTf
MHK/CDvM0nPiG6Kx68XqHIWLFf04qG6rYRWKg8sB1AKKCKZUXp7LSEhuHwKBgCDu
isfw40AHGlvhRfzMoE/+juiG4trpJoqR2qsxiNrbfT/BTbQGygbgz9C38SPGt16F
yBOkFanFz42Mf24i7a7YRfdYGADgcFTYK6NCiYvGzvL68yhLAhcwE2gB+o/QoW85
0WI+jwKvyHU2KQaBVPO0pl73YT7DlPSVYI47sH7BAoGBAJFjXbiEdwDEcDKqRSYs
mGjswFExUD2p8kA5Uj+L3dDQ3gUgjV57lwcMscOg7YBud4I7ZCqIiIwuBUdbs6us
DomjroLjnRBjQ+rk9CFHmB6yMOiayz3h4PNEbJG5e6M61As6W3+DwR99a+lBUoJa
iVMBrpxlsWxDGCbwupVFWmn9
-----END PRIVATE KEY-----
)EOF";

#define SPREADSHEET_ID "1kB_Osier1iGPxEMuVwTzUe5tG0T8UT6uVT-pobRUY5M"
#define TAB_NAME "Sheet1"

// ========== CẤU HÌNH NTP ========== //
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600); // GMT+7 cho Việt Nam

// ========== BIẾN TOÀN CỤC ========== //
float lastPPM = 0.0;
int lastHumidity = 0;
int lastTemperature = 0;
String lastStatus = "Khởi động...";
String prevStatus = "";

// ========== HÀM ĐỊNH DẠNG THỜI GIAN ========== //
String formatDateTime(time_t epochTime) {
  struct tm *timeinfo;
  timeinfo = localtime(&epochTime);
  
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  return String(buffer);
}

// ========== HÀM GỬI LOG LÊN GOOGLE SHEETS ========== //
void sendStatusToGoogleSheets() {
  // Định dạng thời gian
  String formattedTime = formatDateTime(timeClient.getEpochTime());
  
  Serial.print("\n📤 Gửi log | Thời gian: ");
  Serial.print(formattedTime);
  Serial.print(" | PPM: ");
  Serial.print(lastPPM);
  Serial.print(" | Độ ẩm: ");
  Serial.print(lastHumidity);
  Serial.print("% | Nhiệt độ: ");
  Serial.print(lastTemperature);
  Serial.print("°C | Trạng thái: ");
  Serial.println(lastStatus);

  // Tạo dữ liệu gửi đi
  FirebaseJson valueRange;
  valueRange.set("majorDimension", "ROWS");
  
  FirebaseJsonArray row;
  row.add(formattedTime);
  row.add(lastPPM);
  row.add(lastHumidity);
  row.add(lastTemperature);
  row.add(lastStatus);
  
  valueRange.set("values/[0]", row);

  // Gửi dữ liệu
  FirebaseJson response;
  bool success = GSheet.values.append(
    &response, 
    SPREADSHEET_ID, 
    (String(TAB_NAME) + "!A:E").c_str(), 
    &valueRange,
    "USER_ENTERED",
    "INSERT_ROWS"
  );

  if (success) {
    Serial.println("✅ Gửi log thành công!");
  } else {
    Serial.print("❌ Gửi log thất bại: ");
    Serial.println(GSheet.errorReason().c_str());
  }
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, RX, TX); // Khởi động UART với STM32

  // Kết nối WiFi đa mạng
  wifiMulti.addAP("TOTOLINK - tầng 4", "12345sau");
  wifiMulti.addAP("UET-Wifi-Free 2.4Ghz", "");
  wifiMulti.addAP("Hello", "88888888");
  wifiMulti.addAP("Phong 7", "phong7@gmail.com");

  Serial.println("⏳ Đang kết nối WiFi...");
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n✅ Đã kết nối: ");
  Serial.println(WiFi.SSID());
  Serial.print("🌐 Địa chỉ IP: ");
  Serial.println(WiFi.localIP());

  // Kết nối Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
  Serial.println("⌛ Đang kết nối Blynk...");
  while (!Blynk.connected()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Kết nối Blynk thành công!");

  // Đồng bộ thời gian
  Serial.println("⏳ Đang đồng bộ thời gian...");
  timeClient.begin();
  
  int ntpAttempts = 0;
  while (!timeClient.update() && ntpAttempts < 10) {
    timeClient.forceUpdate();
    Serial.print(".");
    delay(500);
    ntpAttempts++;
  }
  
  if (timeClient.isTimeSet()) {
    Serial.print("\n✅ Thời gian hiện tại: ");
    Serial.println(formatDateTime(timeClient.getEpochTime()));
  } else {
    Serial.println("\n❌ Không thể đồng bộ thời gian!");
  }

  // Cấu hình Google Sheets
  GSheet.setPrerefreshSeconds(10 * 60); // 10 phút
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

  Serial.print("🔐 Kiểm tra xác thực Google...");
  if (GSheet.ready()) {
    Serial.println("✅ Thành công!");
  } else {
    Serial.println("\n❌ Xác thực thất bại");
    Serial.print("Lý do: ");
    Serial.println(GSheet.errorReason().c_str());
  }

  // Gửi trạng thái khởi động
  Blynk.virtualWrite(V0, lastStatus);
  Serial.println("🚀 Hệ thống đã sẵn sàng!");
  Serial.println("📡 Đang chờ dữ liệu từ STM32...");
}

void loop() {
  Blynk.run();
  
  // Cập nhật thời gian mỗi phút
  static unsigned long lastTimeUpdate = 0;
  if (millis() - lastTimeUpdate >= 60000) {
    timeClient.update();
    lastTimeUpdate = millis();
  }

  // Xử lý dữ liệu từ UART (STM32)
  if (mySerial.available()) {
    String data = mySerial.readStringUntil('\n');
    data.trim();
    // Serial.println("📩 Nhận từ STM32: " + data); // Có thể bật lại để debug

    // Trích xuất giá trị từ chuỗi dữ liệu
    if (data.startsWith("PPM:")) {
      // Tìm vị trí các dấu phân cách
      int ppmEnd = data.indexOf('|');
      int rhStart = data.indexOf("RH:") + 3;   // "RH:" dài 3 ký tự
      int tempStart = data.indexOf("Temp:") + 5; // "Temp:" dài 5 ký tự

      // Trích xuất PPM
      if (ppmEnd != -1) {
        String ppmStr = data.substring(5, ppmEnd); 
        ppmStr.trim();
        lastPPM = ppmStr.toFloat();
      }

      // Trích xuất độ ẩm (RH)
      if (rhStart != -1) {
        int rhEnd = data.indexOf('|', rhStart);
        if (rhEnd == -1) rhEnd = data.length(); // Nếu không tìm thấy, lấy đến cuối chuỗi
        
        String humStr = data.substring(rhStart, rhEnd);
        humStr.trim();
        lastHumidity = humStr.toInt();
      }

      // Trích xuất nhiệt độ
      if (tempStart != -1) {
        String tempStr = data.substring(tempStart);
        tempStr.trim();
        lastTemperature = tempStr.toInt();
      }

      // Phân loại trạng thái theo mức PPM
      String newStatus;
      if (lastPPM < 300) {
        newStatus = "Bình thường";
      } else if (lastPPM < 500) {
        newStatus = "Có Khí Gas";
      } else if (lastPPM < 1000) {
        newStatus = "Cảnh Báo";
      } else {
        newStatus = "NGUY HIỂM!";
      }

      // Kiểm tra nếu trạng thái thay đổi
      if (newStatus != lastStatus) {
        lastStatus = newStatus;
        Serial.println("==> Trạng thái thay đổi: " + lastStatus);

        // Gửi cảnh báo lên Blynk khi trạng thái thay đổi
        Blynk.virtualWrite(V0, lastStatus);
        
        // Gửi cảnh báo qua event
        if (lastStatus == "Có Khí Gas") {
          Blynk.logEvent("gas_alert", "🔔 Có Khí Gas! PPM: " + String(lastPPM, 2));
        } else if (lastStatus == "Cảnh Báo") {
          Blynk.logEvent("gas_alert", "⚠️ Cảnh Báo! PPM: " + String(lastPPM, 2));
        } else if (lastStatus == "NGUY HIỂM!") {
          Blynk.logEvent("gas_alert", "🚨 NGUY HIỂM! PPM: " + String(lastPPM, 2));
        }

        // Ghi log lên Google Sheets khi trạng thái thay đổi
        if (GSheet.ready()) {
          sendStatusToGoogleSheets();
        } else {
          Serial.println("⚠️ Chưa sẵn sàng gửi log lên Google Sheets");
        }
      }
      
      // Gửi dữ liệu cảm biến liên tục lên Blynk
      //Blynk.virtualWrite(V1, lastPPM);        // PPM
      //Blynk.virtualWrite(V2, lastHumidity);   // Độ ẩm
      //Blynk.virtualWrite(V3, lastTemperature);// Nhiệt độ
      
      // In thông tin dữ liệu hiện tại
      Serial.print("PPM: ");
      Serial.print(lastPPM);
      Serial.print(" | Độ ẩm: ");
      Serial.print(lastHumidity);
      Serial.print("% | Nhiệt độ: ");
      Serial.print(lastTemperature);
      Serial.print("°C | Trạng thái: ");
      Serial.println(lastStatus);
    }
  }

  delay(100);
}