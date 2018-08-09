#include "sepintegralPID.h"
#include <assert.h>
#include <math.h>
#include <QDebug>

SepIntegralPID::SepIntegralPID(double Kp, double Ti, double Td, double threshould) :
    NormalPID(Kp, Ti, Td), integThreshould(threshould)
{

}

SepIntegralPID::~SepIntegralPID()
{

}

double SepIntegralPID::PIDCtrl(double error, double time)
{
	assert(time > 0);
	assert(ti != 0);

	if(time <= 0 && ti == 0)
		return 0;

	if (m_ucMode == 1)//采集到第一个e(k)：位置式PI控U = kp*e+kp*T*e/ti
	{
		if (abs(error) <= integThreshould)
			result = kp*error + kp*time*error / ti;
		else
			result = kp*error;

		lastError = error;
		m_ucMode++;
	}
	else if (m_ucMode == 2)//采集到第二个e(k):位置式PID控U=kp*e1+ki*T*(e1+e0)+kd(e1-e0)/T
	{
		if (abs(error) <= integThreshould)
		{
			if (abs(lastError) <= integThreshould)//两次误差都符合积分阈值以下的条件：正常积分
				result = kp*error + kp*time*(error + lastError) / ti + kp*td*(error - lastError) / time;
			else//第一次误差不符合积分条件：只积分第二次的误差，微分仍然正常计算
				result = kp*error + kp*time*error / ti + kp*td*(error - lastError) / time;
		}
		else
		{
			if (abs(lastError) <= integThreshould)//只有第一次的误差符合积分条件
				result = kp*error + kp*time*lastError / ti + kp*td*(error - lastError) / time;
			else//两次误差都不符合积分条件
				result = kp*error + kp*td*(error - lastError) / time;
		}

		last2Error = lastError;
		lastError = error;
		m_ucMode++;
	}
	else if (m_ucMode >= 3)//采集到第三个及以上e(k)：增量式PID:U(k)=U(k-1)+Δu
		//Δu=kp(ek-ek1)+kp*T*ek/ti+kp*Td(ek-2ek1+ek2)/T
	{
		if( abs(error) <= integThreshould )
			result = result + kp*(error - lastError) + kp*time*error / ti + kp*td*(error - 2 * lastError + last2Error) / time;
		else
			result = result + kp*(error - lastError) + kp*td*(error - 2 * lastError + last2Error) / time;

		last2Error = lastError;
		lastError = error;
	}
	else return 0;

//	qDebug()<<"error:"<<error<<",time:"<<time<<",result:"<<result;
	return result;
}

double SepIntegralPID::PIDCtrlGivenDiff(double error, double diff)
{
	(void) error;//消警告
	(void) diff;
	//	assert(m_dTi != 0);

	//	if (m_ucMode == 1)//采集到第一个e(k)：位置式PID控U=Kp*e+Kp/Ti*e*T+Kp*Td*(diff)
	//	{
	//		if (abs(error) <= integThreshould)
	//			result = m_dKp*error + m_dKp*time*error / m_dTi + m_dKp*m_dTd*diff;
	//		else
	//			result = m_dKp*error + m_dKp*m_dTd*diff;

	//		lastError = error;
	//		m_ucMode++;
	//	}
	//	else if (m_ucMode == 2)//采集到第二个及以上e(k):增量式PID:U(k)=U(k-1)+Δu
	//		//Δu=kp(ek-ek1)+kp*T*ek/ti+kp*Td(ek-2ek1+ek2)/T


	//		//位置式PID控U=kp*e1+ki*T*(e1+e0)+kd(e1-e0)/T
	//	{
	//		if (abs(error) <= integThreshould)
	//		{
	//			if (abs(lastError) <= integThreshould)//两次误差都符合积分阈值以下的条件：正常积分
	//				result = m_dKp*error + m_dKp*time*(error + lastError) / m_dTi + m_dKp*m_dTd*(error - lastError) / time;
	//			else//第一次误差不符合积分条件：只积分第二次的误差，微分仍然正常计算
	//				result = m_dKp*error + m_dKp*time*error / m_dTi + m_dKp*m_dTd*(error - lastError) / time;
	//		}
	//		else
	//		{
	//			if (abs(lastError) <= integThreshould)//只有第一次的误差符合积分条件
	//				result = m_dKp*error + m_dKp*time*lastError / m_dTi + m_dKp*m_dTd*(error - lastError) / time;
	//			else//两次误差都不符合积分条件
	//				result = m_dKp*error + m_dKp*m_dTd*(error - lastError) / time;
	//		}

	//		last2Error = lastError;
	//		lastError = error;
	//		m_ucMode++;
	//	}
	//	else if (m_ucMode >= 3)//采集到第三个及以上e(k)：增量式PID:U(k)=U(k-1)+Δu
	//		//Δu=kp(ek-ek1)+kp*T*ek/ti+kp*Td(ek-2ek1+ek2)/T
	//	{
	//		if( abs(error) <= integThreshould )
	//			result = result + m_dKp*(error - lastError) + m_dKp*time*error / m_dTi + m_dKp*m_dTd*(error - 2 * lastError + last2Error) / time;
	//		else
	//			result = result + m_dKp*(error - lastError) + m_dKp*m_dTd*(error - 2 * lastError + last2Error) / time;

	//		last2Error = lastError;
	//		lastError = error;
	//	}
	//	else return 0;

	return result;
}
