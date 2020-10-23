enum OP {
  TimeBaseBoardcast, // 时基广播
  RootNodeBoardcast, // 根节点广播
  SensorData,        // 传感器数据类型
  NetworkTopology,
  LogonRequest // 根节点登录请求
};

enum MODE {
  Setting, // 设置模式
  Mesh, // Mesh模式
};

enum APDS_TAG {
  Clear, // 环境光
  Red, // 红光波长范围
  Green, // 绿光波长范围
  Blue, // 蓝光波长范围
};

enum BME_TAG {
  TA, // 温度, 高度
  HP, // 湿度, 压强
};
