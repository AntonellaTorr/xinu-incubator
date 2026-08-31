/*
 * ultrasound: controla el sensor HC-SR04 para medir distancias en cm
 */

 #ifndef ULTRASOUND_H
 #define ULTRASOUND_H
 
 void init_ultrasound(int trig, int echo);
 int ultrasound_get_distance(int trig, int echo);
 
 #endif /* ULTRASOUND_H */