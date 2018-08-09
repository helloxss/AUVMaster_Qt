#include "normalPID.h"
#include "assert.h"
#include <qDebug>

NormalPID::NormalPID(double Kp, double Ti, double Td):
	kp(Kp), ti(Ti), td(Td)
{

}

NormalPID::~NormalPID()
{

}

double NormalPID::PIDCtrlGivenDiff(double error, double diff, double time)
{
//	assert(time > 0);
//	if (ti == 0)
//		return PDCtrlGivenDiff(error, diff);

//	static double pResult = 0, iResult = 0, dResult = 0;

//	if(m_ucMode >= 3)//采集到第三个及以上e(k)：增量式PID:U(k)=U(k-1)+Δu
//		//Δu=kp(ek-ek1)+kp*T*ek/ti+kp*Td(ek-2ek1+ek2)/T
//	{
//		pResult = pResult + kp*(error - lastError);
//		iResult = iResult + kp*time*error / ti;
//		dResult = kp * td * diff;
//		result = pResult + iResult + dResult;
//		last2Error = lastError;
//		lastError = error;
//	}
//	else if(m_ucMode == 2)//采集到第二个e(k):位置式PID控U=kp*e1+ki*T*(e1+e0)+kd(e1-e0)/T
//	{
//		pResult = kp*error;
//		iResult = kp*time*(error+lastError) / ti;
//		dResult = kp * td * diff;
//		result = pResult + iResult + dResult;
//		last2Error = lastError;
//		lastError = error;
//		m_ucMode++;
//	}
//	else if(m_ucMode == 1)//采集到第一个e(k)：位置式PI控U = kp*e+kp*T*e/ti
//	{
//		pResult = kp*error;
//		iResult = kp*time*error / ti;
//		result = pResult + iResult;
//		lastError = error;
//		m_ucMode++;
//	}
//	else return 0;

//	return result;

	static double lastDiff = 0;

	assert(time > 0);
	if (ti == 0)
		return PDCtrl(error, time);

	if (m_ucMode == 1)//采集到第一个e(k)：位置式PI控U = kp*e+kp*T*e/ti
	{
		result = kp*error + kp*time*error / ti;
		lastError = error;
		m_ucMode++;
	}
	else if (m_ucMode == 2)//采集到第二个e(k):位置式PID控U=kp*e1+ki*T*(e1+e0)+kd(e1-e0)/T
	{
		result = kp*error + kp*time*(error + lastError) / ti + kp*td*diff;
		lastDiff = diff;
		last2Error = lastError;
		lastError = error;
		m_ucMode++;
	}
	else if (m_ucMode >= 3)//采集到第三个及以上e(k)：增量式PID:U(k)=U(k-1)+Δu
		//Δu=kp(ek-ek1)+kp*T*ek/ti+kp*Td(ek-2ek1+ek2)/T
	{
		result = result + kp*(error - lastError) + kp*time*error / ti + kp*td* /*(error - 2 * lastError + last2Error) / time*/ (diff - lastDiff);
		lastDiff = diff;
		last2Error = lastError;
		lastError = error;
	}
	else return 0;

	return result;
}

double NormalPID::PIDCtrl(double error, double time)
{
	assert(time > 0);
	if (ti == 0)
		return PDCtrl(error, time);

	if (m_ucMode == 1)//采集到第一个e(k)：位置式PI控U = kp*e+kp*T*e/ti
	{
		result = kp*error + kp*time*error / ti;
		lastError = error;
		m_ucMode++;
	}
	else if (m_ucMode == 2)//采集到第二个e(k):位置式PID控U=kp*e1+ki*T*(e1+e0)+kd(e1-e0)/T
	{
		result = kp*error + kp*time*(error + lastError) / ti + kp*td*(error - lastError) / time;
		last2Error = lastError;
		lastError = error;
		m_ucMode++;
	}
	else if (m_ucMode >= 3)//采集到第三个及以上e(k)：增量式PID:U(k)=U(k-1)+Δu
		//Δu=kp(ek-ek1)+kp*T*ek/ti+kp*Td(ek-2ek1+ek2)/T
	{
		result = result + kp*(error - lastError) + kp*time*error / ti + kp*td*(error - 2 * lastError + last2Error) / time;
		last2Error = lastError;
		lastError = error;
	}
	else return 0;

	return result;
}

double NormalPID::PDCtrlGivenDiff(double error, double diff)
{
	//使用外部获得的微分项diff
	//位置式PD：U(k) = kp*(e(k)+td*diff)
	return kp*(error + td*diff);
}

double NormalPID::PDCtrl(double error, double time)
{
	if (m_ucMode == 1)//采集到第一个e(k)：位置式P控U = kp*e
	{
		result = kp*error;
		m_ucMode++;
	}
	else if (m_ucMode >= 2)//采集到第二个e(k):位置式PD控U=kp*e1+kd(e1-e0)/T
	{
		result = kp*error + kp*td*(error - lastError) / time;
	}

	last2Error = lastError;
	lastError = error;

	return result;
}
