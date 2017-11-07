#include "StdAfx.h"
#include "mat.h"

// MATRIX OPERATIONS DEFINITIONS
Mat3 inverse(const Mat3& mat)
{
	// 3x3 Matrix
    // A B C
    // D E F
    // G H I

    // assign meaningful names to the values for readability
#define MATA ( mat[0][0] )
#define MATB ( mat[0][1] )
#define MATC ( mat[0][2] )
#define MATD ( mat[1][0] )
#define MATE ( mat[1][1] )
#define MATF ( mat[1][2] )
#define MATG ( mat[2][0] )
#define MATH ( mat[2][1] )
#define MATI ( mat[2][2] )

	// calculate cofactors
	float Ca = MATE * MATI - MATF * MATH;
	float Cb = -MATD * MATI + MATF * MATG;
	float Cc = MATD * MATH - MATE * MATG;
	float Cd = -MATB * MATI + MATC * MATH;
	float Ce = MATA * MATI - MATC * MATG;
	float Cf = -MATA * MATH + MATB * MATG;
	float Cg = MATB * MATF - MATC * MATE;
	float Ch = -MATA * MATF + MATC * MATD;
	float Ci = MATA * MATE - MATB * MATD;

	// adjoint matrix
	Mat3 adj(Ca, Cd, Cg, Cb, Ce, Ch, Cc, Cf, Ci);

	// determinant from minors
	float det = MATA * Ca + MATB * Cb + MATC * Cc;

	ASSERT(det != 0.f);

	float oneOverDet = 1.f / det;

#undef MATA
#undef MATB
#undef MATC
#undef MATD
#undef MATE
#undef MATF
#undef MATG
#undef MATH
#undef MATI

	return oneOverDet * adj;
}

Mat4 inverse(const Mat4& mat)
{
	// 4x4 Matrix
	// A B C D
	// E F G H
	// I J K L
	// M N O P

	// assign meaningful names to the values for readability
#define MATA ( mat[0][0] )
#define MATB ( mat[0][1] )
#define MATC ( mat[0][2] )
#define MATD ( mat[0][3] )
#define MATE ( mat[1][0] )
#define MATF ( mat[1][1] )
#define MATG ( mat[1][2] )
#define MATH ( mat[1][3] )
#define MATI ( mat[2][0] )
#define MATJ ( mat[2][1] )
#define MATK ( mat[2][2] )
#define MATL ( mat[2][3] )
#define MATM ( mat[3][0] )
#define MATN ( mat[3][1] )
#define MATO ( mat[3][2] )
#define MATP ( mat[3][3] )

	// pre-calculate 2x2 determinants
	float KLOP = MATK * MATP - MATO * MATL;
	float JLNP = MATJ * MATP - MATN * MATL;
	float JKNO = MATJ * MATO - MATN * MATK;
	float  ILMP = MATI * MATP - MATM * MATL;
	float IKMO = MATI * MATO - MATM * MATK;
	float IJMN = MATI * MATN - MATM * MATJ;
	float GHOP = MATG * MATP - MATO * MATH;
	float FHNP = MATF * MATP - MATN * MATH;
	float GHKL = MATG * MATL - MATK * MATH;
	float FGJK = MATF * MATK - MATJ * MATG;
	float FHJL = MATF * MATL - MATJ * MATH;
	float FGNO = MATF * MATO - MATN * MATG;
	float EHMP = MATE * MATP - MATM * MATH;
	float EGMO = MATE * MATO - MATM * MATG;
	float EFMN = MATE * MATN - MATM * MATF;
	float EHIL = MATE * MATL - MATI * MATH;
	float EGIK = MATE * MATK - MATI * MATG;
	float EFIJ = MATE * MATJ - MATI * MATF;

	// calculate cofactors
	float Ca = MATF * KLOP - MATG * JLNP + MATH * JKNO;
	float Cb = -MATE * KLOP + MATG * ILMP - MATH * IKMO;
	float Cc = MATE * JLNP - MATF * ILMP + MATH * IJMN;
	float Cd = -MATE * JKNO + MATF * IKMO - MATG * IJMN;

	float Ce = -MATB * KLOP + MATC * JLNP - MATD * JKNO;
	float Cf = MATA * KLOP - MATC * ILMP + MATD * IKMO;
	float Cg = -MATA * JLNP + MATB * ILMP - MATD * IJMN;
	float Ch = MATA * JKNO - MATB * IKMO + MATC * IJMN;

	float Ci = MATB * GHOP - MATC * FHNP + MATD * FGNO;
	float Cj = -MATA * GHOP + MATC * EHMP - MATD * EGMO;
	float Ck = MATA * FHNP - MATB * EHMP + MATD * EFMN;
	float Cl = -MATA * FGNO + MATB * EGMO - MATC * EFMN;

	float Cm = -MATB * GHKL + MATC * FHJL - MATD * FGJK;
	float Cn = MATA * GHKL - MATC * EHIL + MATD * EGIK;
	float Co = -MATA * FHJL + MATB * EHIL - MATD * EFIJ;
	float Cp = MATA * FGJK - MATB * EGIK + MATC * EFIJ;

	// adjoint matrix
	Mat4 adj(Ca, Ce, Ci, Cm, Cb, Cf, Cj, Cn, Cc, Cg, Ck, Co, Cd, Ch, Cl, Cp);

	// determinant from minors
	float det = MATA * Ca + MATB * Cb + MATC * Cc + MATD * Cd;

	ASSERT(det != 0.f);

	float oneOverDet = 1.f / det;

	// undefine placeholder names
#undef MATA
#undef MATB
#undef MATC
#undef MATD
#undef MATE
#undef MATF
#undef MATG
#undef MATH
#undef MATI
#undef MATJ
#undef MATK
#undef MATL
#undef MATM
#undef MATN
#undef MATO
#undef MATP

	return oneOverDet * adj;
}