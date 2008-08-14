
#ifndef _CD_SIMPLEX
#define _CD_SIMPLEX

#pragma once



#include "../matrix/SCD_Types.h"

namespace SCD
{

	enum CD_SimplexType 
	{
		CD_Point,
		CD_Segment,
		CD_Triangle,
		CD_Tetrahedron,
		CD_None
	};


	/*!
	* tells which points ware kept from the previous simplex
	*/
	struct CD_SimplexKeptPoints
	{
		char b1,b2,b3,b4;

		CD_SimplexType type;

		inline CD_SimplexKeptPoints()
		{
			b1=b2=b3=b4=-1;
		}

		inline char & operator[]( unsigned char i)
		{
			return ((i==0) ? b1: (i==1)? b2: (i==2) ? b3 : b4);
		}

		inline const char & operator[]( unsigned char i) const
		{
			return ((i==0) ? b1: (i==1)? b2: (i==2) ? b3 : b4);
		}

		void reset()
		{
			b1=b2=b3=b4=-1;
		}



	};

	class CD_Simplex
	{
	public:
		CD_Simplex(const Point3& p);
		CD_Simplex(const Point3& p1,const Point3& p2);
		CD_Simplex(const Point3& p1,const Point3& p2,const Point3& p3);
		CD_Simplex(const Point3& p1,const Point3& p2,const Point3& p3,const Point3& p4);


		virtual ~CD_Simplex();





		CD_SimplexType getType()const;







		const Point3& operator[]( unsigned char) const;

		Point3& operator[]( unsigned char);



		CD_Simplex& operator=(const CD_Simplex& s);

		CD_Simplex& operator=(const Point3& p);



		bool operator==(const CD_Simplex& s);

		bool operator!=(const CD_Simplex& s);



		/*! 
		* \brief Adds a point to a simplex to transform it in a higher dimemsion simplex (doesn't work with tetrahedron) 
		* \updateVetors must be called after this operator to keep the vectors up to date.
		*/
		CD_Simplex& operator+=(const Point3&);


		/*! 
		* \brief Adds a point to a simplex to transform it in a higher dimemsion simplex (doesn't work with tetrahedron) 
		* \updateVetors must be called after this operator to keep the vectors up to date.
		*/
		CD_Simplex operator+(const Point3&)const;

		/*! 
		* \brief Updates the simplex by supressing some vertexes and/or change their order according to a filter
		* \param k is the filter used to know wich vertexes will be kept and in wich order
		* \updateVetors must be called after this function to keep the vectors up to date.
		*/
		virtual void filter(const CD_SimplexKeptPoints &k);

		/*!
		*\brief Gives the distance squared at the origin for a simplex, and according the direction v.
		*/
		Scalar squareDistanceAtOrigin(const Vector3 &v)const;








		/*!
		* \brief Updates AB,AC,AD vectors. A is the last vertex inserted and B, C and D are previous vertexes 
		*/
		void updateVectors();


		/*!
		* \brief Returns AB vector 
		*/
		const Vector3& AB () const;

		/*!
		* \brief Returns AC vector
		*/
		const Vector3& AC () const;

		/*!
		* \brief Returns AD vector 
		*/
		const Vector3& AD () const;

	protected :


		CD_SimplexType type_;

		Point3 s1_,s2_,s3_,s4_;
		Vector3 ab_,ac_,ad_;





		static const Scalar zero_;
		static const Scalar zero2_;
		static const Scalar zero3_;
		static const Scalar zero4_;

	};

#include "CD_Simplex.hxx"
}
#endif

