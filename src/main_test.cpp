// 光流传感器数据结构体
struct FlowData {
  int16_t flow_x;
  int16_t flow_y;
  uint32_t integration_time;
  uint16_t distance;
  uint8_t valid;
  uint8_t confidence;
};

// 传输数据结构体
struct VelocityData {
  float x_vel;  // cm/s
  float y_vel;  // cm/s  
  float z_vel;  // m/s
  uint32_t timestamp;
  uint8_t checksum;
};

FlowData current_flow;
VelocityData velocity_data;

// 状态机变量
enum ParseState { WAIT_HEADER, WAIT_LENGTH, PARSE_DATA };
ParseState state = WAIT_HEADER;
uint8_t buffer[14];
uint8_t data_index = 0;

// 配置参数
const float HEIGHT_FACTOR = 1.0f;
const float TIME_INTERVAL = 0.04f;
const float CONVERSION_FACTOR = (HEIGHT_FACTOR / TIME_INTERVAL) * 100.0f;

// 通信配置
const uint32_t BAUD_RATE = 115200;
const int TRANSMIT_INTERVAL = 20; // 传输间隔(ms)

void setup() {
  Serial.begin(BAUD_RATE);        // 调试输出
  Serial1.begin(BAUD_RATE, SERIAL_8N1, 16, 17); // 光流传感器
  Serial2.begin(BAUD_RATE, SERIAL_8N1, 18, 19); // 上位机通信
  
  Serial.println("ESP32 光流传感器节点启动...");
}

void loop() {
  static unsigned long last_transmit = 0;
  
  // 解析传感器数据
  if (parseFlowData()) {
    calculateVelocity();
  }
  
  // 定时向上位机传输数据
  if (millis() - last_transmit >= TRANSMIT_INTERVAL) {
    transmitVelocityData();
    last_transmit = millis();
  }
}

bool parseFlowData() {
  while (Serial1.available()) {
    uint8_t ch = Serial1.read();
    
    switch (state) {
      case WAIT_HEADER:
        if (ch == 0xFE) {
          buffer[0] = ch;
          data_index = 1;
          state = WAIT_LENGTH;
        }
        break;
        
      case WAIT_LENGTH:
        if (ch == 0x0A) {
          buffer[1] = ch;
          data_index = 2;
          state = PARSE_DATA;
        } else {
          state = WAIT_HEADER;
        }
        break;
        
      case PARSE_DATA:
        buffer[data_index++] = ch;
        
        if (data_index >= 14) {
          if (buffer[13] == 0x55 && verifyChecksum()) {
            parseFlowFrame();
            state = WAIT_HEADER;
            return true; // 成功解析一帧数据
          }
          state = WAIT_HEADER;
        }
        break;
    }
  }
  return false;
}

bool verifyChecksum() {
  uint8_t checksum = 0;
  for (int i = 2; i < 12; i++) {
    checksum ^= buffer[i];
  }
  return checksum == buffer[12];
}

void parseFlowFrame() {
  current_flow.flow_x = (int16_t)((buffer[3] << 8) | buffer[2]);
  current_flow.flow_y = (int16_t)((buffer[5] << 8) | buffer[4]);
  current_flow.integration_time = (buffer[7] << 8) | buffer[6];
  current_flow.distance = (buffer[9] << 8) | buffer[8];
  current_flow.valid = buffer[10];
  current_flow.confidence = buffer[11];
}

void calculateVelocity() {
  if (current_flow.valid == 0xF5) {
    float flow_x_actual = -current_flow.flow_x / 10000.0f;
    float flow_y_actual = -current_flow.flow_y / 10000.0f;
    
    velocity_data.x_vel  = flow_x_actual * CONVERSION_FACTOR;
    velocity_data.y_vel = flow_y_actual * CONVERSION_FACTOR;
    
    // 速度限幅
    velocity_data.x_vel = constrain(velocity_data.x_vel, -62.0f, 62.0f);
    velocity_data.y_vel = constrain(velocity_data.y_vel, -62.0f, 62.0f);
    
    // Z轴速度计算
    static float prev_z_pos = 0;
    float current_z_pos = current_flow.distance / 1000.0f;
    velocity_data.z_vel = (current_z_pos - prev_z_pos) / (TIME_INTERVAL);
    prev_z_pos = current_z_pos;
    
    velocity_data.timestamp = millis();
  }
}

void transmitVelocityData() {
  // 计算校验和
  velocity_data.checksum = calculateVelocityChecksum();
  
  // 通过Serial2发送给上位机
  Serial2.write((uint8_t*)&velocity_data, sizeof(VelocityData));
  
  // 调试输出
  Serial.printf("发送速度数据: X:%.2f Y:%.2f Z:%.3f\n", 
                velocity_data.x_vel, velocity_data.y_vel, velocity_data.z_vel);
}

uint8_t calculateVelocityChecksum() {
  uint8_t checksum = 0;
  uint8_t* data = (uint8_t*)&velocity_data;
  
  for (size_t i = 0; i < sizeof(VelocityData) - 1; i++) {
    checksum ^= data[i];
  }
  return checksum;
}