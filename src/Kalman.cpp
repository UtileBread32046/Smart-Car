#include "Kalman.h"

// 更新函数, 输入传感器测量值, 输出最优估计值
double KalmanFilter::updateKalman(double measurement) {
  P = P + Q; // 1. 预测, 累计误差程度
  double K = P / (P+R); // 2. 计算权重, 平衡误差和测量结果
  estimation = estimation + K*(measurement-estimation); // 3. 修正, 根据计算出的卡尔曼增益修正估计值
  P = (1-K)*P; // 4. 校准, 根据卡尔曼增益重新评估误差程度, 防止无限增大
  return estimation;
}