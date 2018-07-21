#ifndef SEPINTEGRALPID_H
#define SEPINTEGRALPID_H

#include "normalPID.h"

/* 积分分离PID 类
 *
 *
 */
class SepIntegralPID : public NormalPID
{
public:
	SepIntegralPID(double Kp, double Ti, double Td, double threshould);
	~SepIntegralPID();

	double PIDCtrl(double error, double time);//输出PID控制结果
	double PIDCtrlGivenDiff(double error, double diff);//给定微分项的PID控制
	void setIntegThreshould(double threshould){ integThreshould = threshould; }//设定积分分离参数

private:
	double integThreshould = 0;//开始积分的误差阈值（大于此阈值时不进行积分）,等于0时与正常PID相同
};

#endif // SEPINTEGRALPID_H
