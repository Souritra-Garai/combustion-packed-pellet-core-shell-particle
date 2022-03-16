#ifndef __LU_SOLVER__
#define __LU_SOLVER__

template<typename real_t>
class LUSolver
{
	private:

		// size of x vector for the linear algebra problem A.x = b
		const unsigned int N;

		// pointer to array of real type numbers representing 
		// the main diagonal of the tridiagonal matrix a_(i,i)
		real_t *A_matrix_diagonal;
		// the immediate upper diagonal of the tridiagonal matrix a_(i,i+1)
		// Entry at index N-1 is insignificant
		real_t *A_matrix_diagonal_plus_1;
		// the immediate lower diagonal of the tridiagonal matrix a_(i,i-1)
		// Entry at index 0 is insignificant
		real_t *A_matrix_diagonal_less_1;

		// pointer to array of real type numbers representing
		// constant vector b in the matrix equation
		real_t *b;

		// helper function to decompose the A matrix in Upper and Lower matrices
		void LU_Decomposition();

	public:

		LUSolver(unsigned int N);

		~LUSolver();

        /**
         * @brief Set up equation represented by ith
         * row of the matrix equation
         * \f$ e x_{i-1} + f x_i + g x_{i+1} = b \f$
         * 
         * @param index i
         * @param e Coefficient to \f$ x_{i-1} \f$
         * @param f Coefficient to \f$ x_{i} \f$
         * @param g Coefficient to \f$ x_{i+1} \f$
         * @param b Constant
         */
        void setEquation(
            unsigned int index,
            real_t e,
            real_t f,
            real_t g,
            real_t b
        );

        /**
         * @brief Set up equation represented by the first row
         * of the matrix equation
         * \f$ f x_i + g x_{i+1} = b \f$
         * 
         * @param f Coefficient to \f$ x_{i} \f$
         * @param g Coefficient to \f$ x_{i+1} \f$
         * @param b Constant
         */
        void setEquationFirstRow(
            real_t f,
            real_t g,
            real_t b
        );

        /**
         * @brief up equation represented by the last row
         * of the matrix equation
         * \f$ e x_{i-1} f x_i = b \f$
         * 
         * @param e Coefficient to \f$ x_{i-1} \f$
         * @param f Coefficient to \f$ x_{i} \f$
         * @param b Constant
         */
        void setEquationLastRow(
            real_t e,
            real_t f,
            real_t b
        );

        /**
         * @brief Prints the matrix \f$ A \f$ and vector \f$ b \f$
         */
        void printMatrixEquation();

        /**
         * @brief Finds the solution to matrix equation and saves
         * it to array x
         * 
         * @param x Array to store the solution of the matrix equation
         */
        void getSolution(real_t *x);
};

#endif