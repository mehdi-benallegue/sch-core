#ifndef _CD_DEPTH
#define _CD_DEPTH

#pragma once
#include <SCD/scd_api.h>
#include <SCD/S_Object/S_Object.h>
#include <SCD/CD/CD_SimplexEnhanced.h>

namespace SCD
{

	class CD_Depth
	{

	protected:
		S_Object *sObj1_;
		S_Object *sObj2_;

		Scalar precision_,epsilon_;

	public:

		SCD_API CD_Depth(S_Object *Obj1, S_Object *Obj2);
		SCD_API virtual ~CD_Depth(void);

		/*!
		*\brief sets the relative precision of the proximity queries to a given value. Default is 1e-3
		*/
		SCD_API void setRelativePrecision(Scalar);

		/*
		*\brief Sets the machine epsilon. Default is 1e-24
		*/
		SCD_API void setEpsilon(Scalar);

		SCD_API Scalar getPenetrationDepth(Vector3& v, Point3& p1, Point3& p2, const CD_SimplexEnhanced& s,const CD_Simplex& s1_,const CD_Simplex& s2_);
	};

}

#endif
