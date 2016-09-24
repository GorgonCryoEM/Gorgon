//
// Created by shadow_walker on 9/23/16.
//


// *************************************************************************************************
// Code below can go in cpp file
// *************************************************************************************************
using namespace EMAN;

#ifdef FFTW_PLAN_CACHING
// The only thing important about these constants is that they don't equal each other
const int EMfft::EMAN2_REAL_2_COMPLEX = 1;
const int EMfft::EMAN2_COMPLEX_2_REAL = 2;

// It is important that these constants are 1 and 0, exactly as they are here
const int EMfft::EMAN2_FFTW2_INPLACE = 1;
const int EMfft::EMAN2_FFTW2_OUT_OF_PLACE=0;


#ifdef FFTW3
EMfft::EMfftw3_cache::EMfftw3_cache() :
		num_plans(0)
{
	for(int i = 0; i < EMFFTW3_CACHE_SIZE; ++i)
	{
		rank[i] = 0;
		plan_dims[i][0] = 0; plan_dims[i][1] = 0; plan_dims[i][2] = 0;
		r2c[i] = -1;
		ip[i] = -1;
		fftwplans[i] = NULL;
	}
}

void EMfft::EMfftw3_cache::debug_plans()
{
	for(int i = 0; i < EMFFTW3_CACHE_SIZE; ++i)
	{
		cout << "Plan " << i << " has dims " << plan_dims[i][0] << " "
				<< plan_dims[i][1] << " " <<
				plan_dims[i][2] << ", rank " <<
				rank[i] << ", rc flag "
				<< r2c[i] << ", ip flag " << ip[i] << endl;
	}
}

EMfft::EMfftw3_cache::~EMfftw3_cache()
{
	for(int i = 0; i < EMFFTW3_CACHE_SIZE; ++i)
	{
		if (fftwplans[i] != NULL)
		{
			fftwf_destroy_plan(fftwplans[i]);
			fftwplans[i] = NULL;
		}
	}
}

fftwf_plan EMfft::EMfftw3_cache::get_plan(const int rank_in, const int x, const int y, const int z, const int r2c_flag, const int ip_flag, fftwf_complex* complex_data, float* real_data )
{

	if ( rank_in > 3 || rank_in < 1 ) throw InvalidValueException(rank_in, "Error, can not get an FFTW plan using rank out of the range [1,3]");
	if ( r2c_flag != EMAN2_REAL_2_COMPLEX && r2c_flag != EMAN2_COMPLEX_2_REAL ) throw InvalidValueException(r2c_flag, "The real two complex flag is not supported");

// 	static int num_added = 0;
// 	cout << "Was asked for " << rank_in << " " << x << " " << y << " " << z << " " << r2c_flag << endl;

	int dims[3];
	dims[0] = z;
	dims[1] = y;
	dims[2] = x;

	// First check to see if we already have the plan
	int i;
	for (i=0; i<num_plans; i++) {
		if (plan_dims[i][0]==x && plan_dims[i][1]==y && plan_dims[i][2]==z
				  && rank[i]==rank_in && r2c[i]==r2c_flag && ip[i]==ip_flag) return fftwplans[i];
	}

	fftwf_plan plan;
	// Create the plan
	if ( y == 1 && z == 1 )
	{
		if ( r2c_flag == EMAN2_REAL_2_COMPLEX )
			plan = fftwf_plan_dft_r2c_1d(x, real_data, complex_data, FFTW_ESTIMATE);
		else // r2c_flag == EMAN2_COMPLEX_2_REAL, this is guaranteed by the error checking at the beginning of the function
			plan = fftwf_plan_dft_c2r_1d(x, complex_data, real_data, FFTW_ESTIMATE);
	}
	else
	{
		if ( r2c_flag == EMAN2_REAL_2_COMPLEX )
			plan = fftwf_plan_dft_r2c(rank_in, dims + (3 - rank_in), real_data, complex_data, FFTW_ESTIMATE);
		else // r2c_flag == EMAN2_COMPLEX_2_REAL, this is guaranteed by the error checking at the beginning of the function
			plan = fftwf_plan_dft_c2r(rank_in, dims + (3 - rank_in), complex_data, real_data, FFTW_ESTIMATE);
	}

	if (fftwplans[EMFFTW3_CACHE_SIZE-1] != NULL )
	{
		fftwf_destroy_plan(fftwplans[EMFFTW3_CACHE_SIZE-1]);
		fftwplans[EMFFTW3_CACHE_SIZE-1] = NULL;
	}

	int upper_limit = num_plans;
	if ( upper_limit == EMFFTW3_CACHE_SIZE ) upper_limit -= 1;
	for (int i=upper_limit-1; i>0; i--)
	{
		fftwplans[i]=fftwplans[i-1];
		rank[i]=rank[i-1];
		r2c[i]=r2c[i-1];
		ip[i]=ip[i-1];
		plan_dims[i][0]=plan_dims[i-1][0];
		plan_dims[i][1]=plan_dims[i-1][1];
		plan_dims[i][2]=plan_dims[i-1][2];
	}
		//dimplan[0]=-1;

	plan_dims[0][0]=x;
	plan_dims[0][1]=y;
	plan_dims[0][2]=z;
	r2c[0]=r2c_flag;
	ip[0]=ip_flag;
	fftwplans[0] = plan;
	rank[0]=rank_in;
	if (num_plans<EMFFTW3_CACHE_SIZE) num_plans++;
// 			debug_plans();
// 			cout << "Created plan 0" << endl;
// 	++num_added;
// 	cout << "I have created " << num_added << " plans" << endl;
	return fftwplans[0];

}

// Static init
EMfft::EMfftw3_cache EMfft::plan_cache;

#endif // FFTW3
#endif // FFTW_PLAN_CACHING

#ifdef FFTW3

int EMfft::real_to_complex_1d(float *real_data, float *complex_data, int n)
{//cout<<"doing fftw3"<<endl;
#ifdef FFTW_PLAN_CACHING
	bool ip = ( complex_data == real_data );
	fftwf_plan plan = plan_cache.get_plan(1,n,1,1,EMAN2_REAL_2_COMPLEX,ip,(fftwf_complex *) complex_data, real_data);
	// According to FFTW3, this is making use of the "guru" interface - this is necessary if plans are to be reused
	fftwf_execute_dft_r2c(plan, real_data,(fftwf_complex *) complex_data);
#else
	fftwf_plan plan = fftwf_plan_dft_r2c_1d(n, real_data, (fftwf_complex *) complex_data,
											FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);
#endif // FFTW_PLAN_CACHING
	return 0;
}

int EMfft::complex_to_real_1d(float *complex_data, float *real_data, int n)
{
#ifdef FFTW_PLAN_CACHING
	bool ip = ( complex_data == real_data );
	fftwf_plan plan = plan_cache.get_plan(1,n,1,1,EMAN2_COMPLEX_2_REAL,ip,(fftwf_complex *) complex_data, real_data);
	// According to FFTW3, this is making use of the "guru" interface - this is necessary if plans are to be reused
	fftwf_execute_dft_c2r(plan, (fftwf_complex *) complex_data, real_data);
#else
	fftwf_plan plan = fftwf_plan_dft_c2r_1d(n, (fftwf_complex *) complex_data, real_data,
											FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);
#endif // FFTW_PLAN_CACHING

	return 0;
}

int EMfft::real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny, int nz)
{
	const int rank = get_rank(ny, nz);
	int dims[3];
	dims[0] = nz;
	dims[1] = ny;
	dims[2] = nx;

	switch(rank) {
		case 1:
			real_to_complex_1d(real_data, complex_data, nx);
			break;

		case 2:
		case 3:
		{
#ifdef FFTW_PLAN_CACHING
			bool ip = ( complex_data == real_data );
			fftwf_plan plan = plan_cache.get_plan(rank,nx,ny,nz,EMAN2_REAL_2_COMPLEX,ip,(fftwf_complex *) complex_data, real_data);
			// According to FFTW3, this is making use of the "guru" interface - this is necessary if plans are to be re-used
			fftwf_execute_dft_r2c(plan, real_data,(fftwf_complex *) complex_data );
#else
			fftwf_plan plan = fftwf_plan_dft_r2c(rank, dims + (3 - rank),
					real_data, (fftwf_complex *) complex_data, FFTW_ESTIMATE);
			fftwf_execute(plan);
			fftwf_destroy_plan(plan);
#endif // FFTW_PLAN_CACHING
		}
			break;

		default:
			//LOGERR("Should NEVER be here!!!");
			cout << "The rank of the array is not 1D, 2D, or 3D!\n";
			break;
	}

	return 0;
}

int EMfft::complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny, int nz)
{
	const int rank = get_rank(ny, nz);
	int dims[3];
	dims[0] = nz;
	dims[1] = ny;
	dims[2] = nx;

	switch(rank) {
		case 1:
			complex_to_real_1d(complex_data, real_data, nx);
			break;

		case 2:
		case 3:
		{
#ifdef FFTW_PLAN_CACHING
			bool ip = ( complex_data == real_data );
			fftwf_plan plan = plan_cache.get_plan(rank,nx,ny,nz,EMAN2_COMPLEX_2_REAL,ip,(fftwf_complex *) complex_data, real_data);
			// According to FFTW3, this is making use of the "guru" interface - this is necessary if plans are to be re-used
			fftwf_execute_dft_c2r(plan, (fftwf_complex *) complex_data, real_data);
#else
			fftwf_plan plan = fftwf_plan_dft_c2r(rank, dims + (3 - rank),
					(fftwf_complex *) complex_data, real_data, FFTW_ESTIMATE);
			fftwf_execute(plan);
			fftwf_destroy_plan(plan);
#endif // FFTW_PLAN_CACHING


		}
			break;

		default:
			//LOGERR("Should NEVER be here!!!");
			cout << "The rank of the array is not 1D, 2D, or 3D!\n";
			break;
	}

	return 0;
}


#endif	//FFTW3
