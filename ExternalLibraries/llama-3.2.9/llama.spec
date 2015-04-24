Summary: Llama library of applied mathematics algorithms
Name: llama
Version: 3.2.9
Release: 2
Source0: %{name}-%{version}.tar.gz
License: GPL
Group: Sciences/Mathematics
BuildRoot: %{_builddir}/%{name}-root

%description
The Llama library of applied mathematics algorithms is a collection 
of C++ wrappers around highly efficient algorithms for applied 
mathematics workhorses for linear algebra, dynamical systems 
(including differential equations), and the Fourier transform.  It 
makes use of LAPACK and ATLAS for fast, efficient linear algebra, 
ODESSA for integration of ordinary differential equations, and FFTW 
for fast computation of Fourier transforms.
Homepage: http://www.tiem.utk.edu/~king/pub/llama

%prep
%setup -q

%build
./configure
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT/usr/local install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/local/include/llama/llama.h
/usr/local/include/llama/Decls.h
/usr/local/include/llama/General.h
/usr/local/include/llama/Exception.h
/usr/local/include/llama/AbstMatrix.h
/usr/local/include/llama/TinyVector.h
/usr/local/include/llama/Matrix.h
/usr/local/include/llama/SMClosure.h
/usr/local/include/llama/MMClosure.h
/usr/local/include/llama/GenMatrix.h
/usr/local/include/llama/RealMatrix.h
/usr/local/include/llama/ComplexMatrix.h
/usr/local/include/llama/Transpose.h
/usr/local/include/llama/Range.h
/usr/local/include/llama/Submatrix.h
/usr/local/include/llama/Diagonal.h
/usr/local/include/llama/FAClosure.h
/usr/local/include/llama/Random.h
/usr/local/include/llama/EigenDecomposition.h
/usr/local/include/llama/LUDecomposition.h
/usr/local/include/llama/SVDecomposition.h
/usr/local/include/llama/QRDecomposition.h
/usr/local/include/llama/CholeskyDecomposition.h
/usr/local/include/llama/Statistics.h
/usr/local/include/llama/AbstDiffeomorphism.h
/usr/local/include/llama/Diffeomorphism.h
/usr/local/include/llama/AbstMapping.h
/usr/local/include/llama/Mapping.h
/usr/local/include/llama/AbstVectorfield.h
/usr/local/include/llama/Vectorfield.h
/usr/local/include/llama/Odessa.h
/usr/local/include/llama/Minimization.h
/usr/local/include/llama/Fourier.h
/usr/local/include/llama/f77fun.h
/usr/local/include/llama/machinu.h
/usr/local/include/llama.h
/usr/local/lib/liblama.so.2.0.1
/usr/local/lib/liblama.so.2
/usr/local/lib/liblama.so
/usr/local/lib/liblama.la
/usr/local/lib/liblama.a

%doc %attr(0444,root,root)
%doc /usr/local/doc/llama/AbstDiffeomorphism_h.html
%doc /usr/local/doc/llama/AbstMapping_h.html
%doc /usr/local/doc/llama/AbstMatrix_h.html
%doc /usr/local/doc/llama/AbstVectorfield_h.html
%doc /usr/local/doc/llama/all-globals.html
%doc /usr/local/doc/llama/CholeskyDecomposition_h.html
%doc /usr/local/doc/llama/ComplexMatrix_h.html
%doc /usr/local/doc/llama/Decls_h.html
%doc /usr/local/doc/llama/Diagonal_h.html
%doc /usr/local/doc/llama/Diffeomorphism_h.html
%doc /usr/local/doc/llama/EigenDecomposition_h.html
%doc /usr/local/doc/llama/Exception_h.html
%doc /usr/local/doc/llama/f77fun_h.html
%doc /usr/local/doc/llama/FAClosure_h.html
%doc /usr/local/doc/llama/Fourier_h.html
%doc /usr/local/doc/llama/full-list-Llama__AbstDiffeomorphism.html
%doc /usr/local/doc/llama/full-list-Llama__AbstMapping.html
%doc /usr/local/doc/llama/full-list-Llama__AbstMatrix.html
%doc /usr/local/doc/llama/full-list-Llama__AbstVectorfield.html
%doc /usr/local/doc/llama/full-list-Llama__CAbstMatrix.html
%doc /usr/local/doc/llama/full-list-Llama__CholeskyDecomposition.html
%doc /usr/local/doc/llama/full-list-Llama__complexify.html
%doc /usr/local/doc/llama/full-list-Llama__Diagonal.html
%doc /usr/local/doc/llama/full-list-Llama__DiagonalMatrix.html
%doc /usr/local/doc/llama/full-list-Llama__Diffeomorphism.html
%doc /usr/local/doc/llama/full-list-Llama__DimError.html
%doc /usr/local/doc/llama/full-list-Llama__DivideError.html
%doc /usr/local/doc/llama/full-list-Llama__EigenDecomposition.html
%doc /usr/local/doc/llama/full-list-Llama__Exception.html
%doc /usr/local/doc/llama/full-list-Llama__FAMClosure.html
%doc /usr/local/doc/llama/full-list-Llama__FourierTransform.html
%doc /usr/local/doc/llama/full-list-Llama__Hilbert.html
%doc /usr/local/doc/llama/full-list-Llama.html
%doc /usr/local/doc/llama/full-list-Llama__IndexError.html
%doc /usr/local/doc/llama/full-list-Llama__IOError.html
%doc /usr/local/doc/llama/full-list-Llama__linspace.html
%doc /usr/local/doc/llama/full-list-Llama__LogicError.html
%doc /usr/local/doc/llama/full-list-Llama__LUDecomposition.html
%doc /usr/local/doc/llama/full-list-Llama__Mapping.html
%doc /usr/local/doc/llama/full-list-Llama__Matrix.html
%doc /usr/local/doc/llama/full-list-Llama__MMClosure.html
%doc /usr/local/doc/llama/full-list-Llama__NormalRandom.html
%doc /usr/local/doc/llama/full-list-Llama__OutOfMemory.html
%doc /usr/local/doc/llama/full-list-Llama__QRDecomposition.html
%doc /usr/local/doc/llama/full-list-Llama__Random.html
%doc /usr/local/doc/llama/full-list-Llama__Range.html
%doc /usr/local/doc/llama/full-list-Llama__SchurPClosure.html
%doc /usr/local/doc/llama/full-list-Llama__SchurQClosure.html
%doc /usr/local/doc/llama/full-list-Llama__SingularMatrix.html
%doc /usr/local/doc/llama/full-list-Llama__SMClosure.html
%doc /usr/local/doc/llama/full-list-Llama__Submatrix.html
%doc /usr/local/doc/llama/full-list-Llama__SVDecomposition.html
%doc /usr/local/doc/llama/full-list-Llama__TinyVector.html
%doc /usr/local/doc/llama/full-list-Llama__Transpose.html
%doc /usr/local/doc/llama/full-list-Llama__UniformRandom.html
%doc /usr/local/doc/llama/full-list-Llama__Vectorfield.html
%doc /usr/local/doc/llama/General_h.html
%doc /usr/local/doc/llama/GenMatrix_h.html
%doc /usr/local/doc/llama/header-list.html
%doc /usr/local/doc/llama/hier.html
%doc /usr/local/doc/llama/index.html
%doc /usr/local/doc/llama/index-long.html
%doc /usr/local/doc/llama/llama2.gif
%doc /usr/local/doc/llama/Llama__AbstDiffeomorphism.html
%doc /usr/local/doc/llama/Llama__AbstMapping.html
%doc /usr/local/doc/llama/Llama__AbstMatrix.html
%doc /usr/local/doc/llama/Llama__AbstVectorfield.html
%doc /usr/local/doc/llama/Llama__CAbstMatrix.html
%doc /usr/local/doc/llama/Llama__CholeskyDecomposition.html
%doc /usr/local/doc/llama/Llama__complexify.html
%doc /usr/local/doc/llama/Llama__Diagonal.html
%doc /usr/local/doc/llama/Llama__DiagonalMatrix.html
%doc /usr/local/doc/llama/Llama__Diffeomorphism.html
%doc /usr/local/doc/llama/Llama__DimError.html
%doc /usr/local/doc/llama/Llama__DivideError.html
%doc /usr/local/doc/llama/Llama__EigenDecomposition.html
%doc /usr/local/doc/llama/Llama__Exception.html
%doc /usr/local/doc/llama/Llama__FAMClosure.html
%doc /usr/local/doc/llama/Llama__FourierTransform.html
%doc /usr/local/doc/llama/llama_h.html
%doc /usr/local/doc/llama/Llama__Hilbert.html
%doc /usr/local/doc/llama/Llama.html
%doc /usr/local/doc/llama/Llama__IndexError.html
%doc /usr/local/doc/llama/Llama__IOError.html
%doc /usr/local/doc/llama/Llama__linspace.html
%doc /usr/local/doc/llama/Llama__LogicError.html
%doc /usr/local/doc/llama/Llama__LUDecomposition.html
%doc /usr/local/doc/llama/Llama__Mapping.html
%doc /usr/local/doc/llama/Llama__Matrix.html
%doc /usr/local/doc/llama/Llama__MMClosure.html
%doc /usr/local/doc/llama/Llama__NormalRandom.html
%doc /usr/local/doc/llama/Llama__OutOfMemory.html
%doc /usr/local/doc/llama/Llama__QRDecomposition.html
%doc /usr/local/doc/llama/Llama__Random.html
%doc /usr/local/doc/llama/Llama__Range.html
%doc /usr/local/doc/llama/Llama__SchurPClosure.html
%doc /usr/local/doc/llama/Llama__SchurQClosure.html
%doc /usr/local/doc/llama/Llama__SingularMatrix.html
%doc /usr/local/doc/llama/Llama__SMClosure.html
%doc /usr/local/doc/llama/Llama__Submatrix.html
%doc /usr/local/doc/llama/Llama__SVDecomposition.html
%doc /usr/local/doc/llama/Llama__TinyVector.html
%doc /usr/local/doc/llama/Llama__Transpose.html
%doc /usr/local/doc/llama/Llama__UniformRandom.html
%doc /usr/local/doc/llama/Llama__Vectorfield.html
%doc /usr/local/doc/llama/LUDecomposition_h.html
%doc /usr/local/doc/llama/Mapping_h.html
%doc /usr/local/doc/llama/Matrix_h.html
%doc /usr/local/doc/llama/Minimization_h.html
%doc /usr/local/doc/llama/MMClosure_h.html
%doc /usr/local/doc/llama/Odessa_h.html
%doc /usr/local/doc/llama/QRDecomposition_h.html
%doc /usr/local/doc/llama/Random_h.html
%doc /usr/local/doc/llama/Range_h.html
%doc /usr/local/doc/llama/RealMatrix_h.html
%doc /usr/local/doc/llama/SMClosure_h.html
%doc /usr/local/doc/llama/Statistics_h.html
%doc /usr/local/doc/llama/Submatrix_h.html
%doc /usr/local/doc/llama/SVDecomposition_h.html
%doc /usr/local/doc/llama/TinyVector_h.html
%doc /usr/local/doc/llama/Transpose_h.html
%doc /usr/local/doc/llama/Vectorfield_h.html
