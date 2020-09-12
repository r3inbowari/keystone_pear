const int const_length = 50;

void LogDivisionSta(String msg) {
  int len = const_length - msg.length();
  String prefix_hs = "";
  for (int i = 0; i < len / 2; i++) {
    prefix_hs += "-";
  }
  Serial.println(prefix_hs + " " + msg + " STA " + prefix_hs);
}

void LogDivisionEnd(String msg) {
  int len = const_length - msg.length();
  String prefix_hs = "";
  for (int i = 0; i < len / 2; i++) {
    prefix_hs += "-";
  }
  Serial.println(prefix_hs + " " + msg + " END " + prefix_hs);
  Serial.println("");
  Serial.println("");
}
