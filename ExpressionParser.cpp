#include <iostream>
#include <cstring>
#include <cstdlib>

bool evaluate( const char* expression, int& result );
bool isNumber( const char character );
bool isOperator( const char character );
bool operate( const int lhs, const char op, const int rhs, int& result );
void stripWhitespace( const char* expression, char* result );
bool evaluateParentheses( const char* expression, char* result );

const int MAX_EXPR_SIZE = 512;

int main( int argc, char* argv[] )
{
	// Print intro text and ask for expression
	std::cout << "Expression Evaluator" << std::endl;
	std::cout << "====================" << std::endl;
	std::cout << "Enter Expression ( Valid characters: 0-9, +-*/() ):" << std::endl << std::endl;

	// Extract expression from standard input and store in a char array
	char expression[ MAX_EXPR_SIZE ];
	std::cin.getline( expression, MAX_EXPR_SIZE );
	std::cout << "=" << std::endl;

	// Evaluate the expression
	int result;
	if( evaluate( expression, result ) ) {
		std::cout << result;
	}
	else
	{
		std::cout << "Invalid expression" << std::endl;
	}

	// Wait for newline and end the program
	std::cin.ignore();
	return 0;
}

// Mathematically evaluates expression and loads the output into result
// Returns false if expression has invalid syntax, true otherwise
bool evaluate( const char* expression, int& result ) {
	// Strip whitespace
	char exp_stripped[ MAX_EXPR_SIZE ];
	stripWhitespace( expression, exp_stripped );

	// Return false if the expression is empty
	if( exp_stripped[ 0 ] == 0 )
	{
		return false;
	}

	// Check for parentheses
	char exp_simplified[ MAX_EXPR_SIZE ];
	if( !evaluateParentheses( exp_stripped, exp_simplified ) )
	{
		return false;
	}

	// Iterate through the expression and solve
	int currentVal = 0;
	char currentOp = '+';

	int solveIdx = 0;
	while( exp_simplified[ solveIdx ] != 0 ) // Iterate until terminating character
	{
		if( isNumber( exp_simplified[ solveIdx ] ) ) // Character is a number
		{
			// Concatenate number into it's own char array
			char concatNum[ MAX_EXPR_SIZE ]; // Using MAX_EXPR_SIZE for safety
			concatNum[ 0 ] = exp_simplified[ solveIdx ]; // We know the first character

			int concatIdx = 1;
			while( isNumber( exp_simplified[ solveIdx + 1 ] ) )
			{
				concatNum[ concatIdx ] = exp_simplified[ solveIdx + 1 ];
				concatIdx++;
				solveIdx++;
			}
			concatNum[ concatIdx ] = 0; // Append terminating character 

			// Get the integer represented by the concatenated c-string number,
			// operate on current value w/next value, return false if unsuccessful
			int nextVal = atoi( concatNum );
			if( !operate( currentVal, currentOp, nextVal, currentVal ) )
			{
				return false;
			}
		}
		else if( isOperator( exp_simplified[ solveIdx ] ) ) // Character is an operator
		{
			// Store as pending operation
			currentOp = exp_simplified[ solveIdx ];
		}
		else // Character is invalid
		{
			return false;
		}

		++solveIdx;
	}

	result = currentVal;
	return true;
}



// Performs (lhs <op> rhs) and loads the output into result
// Returns false if an invalid operator is supplied, true otherwise
bool operate( const int lhs, const char op, const int rhs, int& result )
{
	switch( op )
	{
	case '+':
		result = lhs + rhs;
		return true;
	case '-':
		result = lhs - rhs;
		return true;
		break;
	case '*':
		result = lhs * rhs;
		return true;
	case '/':
		result = lhs / rhs;
		return true;
	default:
		return false;
	}
}

// Strips the whitespace from str, appends a terminating character and returns result
void stripWhitespace( const char* str, char* result )
{
	int stripSize = 0;
	
	int stripIdx = 0;
	while( str[ stripIdx ] != 0 ) // Iterate until terminating character
	{
		if( int( str[ stripIdx ] ) != 32 )
		{
			result[ stripSize ] = str[ stripIdx ];
			++stripSize;
		}
		++stripIdx;
	}

	// Append terminating character
	result[ stripSize ] = '\0';
}

// Locates pairs of parentheses in expression and replaces then with their solution using evaluate()
// Returns false on invalid syntax, true otherwise
bool evaluateParentheses( const char* expression, char* result )
{
	// Copy expression into result so we can work with it directly and avoid no-op issues
	strcpy( result, expression );

	// Iterate backwards over the string,
	// keeping track of the last close/first open parentheses if any are found
	bool parens = true;
	while( parens ) {
		int closeIdx = -1;
		int openIdx = -1;

		int parenCheckIdx = strlen( result ) - 1;
		while( parenCheckIdx >= 0 )
		{
			if( result[ parenCheckIdx ] == ')' )
			{
				closeIdx = parenCheckIdx;
			}
			else if( result[ parenCheckIdx ] == '(' )
			{
				if( closeIdx != -1 )
				{
					openIdx = parenCheckIdx;
					break;
				}
				else
				{
					return false;
				}
			}
			--parenCheckIdx;
		}
	
		// If parenthesis are present, recursively evaluate them and replace with the solution
		if( openIdx != -1 && closeIdx != -1 )
		{
			// Extract the parenthesised sub-result
			int firstChar = openIdx + 1;
			int lastChar = closeIdx;
			int length = lastChar - firstChar;
			char* subExpr = new char[ length + 1 ];

			strncpy( subExpr, result + firstChar, length );
			subExpr[ length ] = '\0';

			// Recursively evaluate
			int solution;
			if( !evaluate( subExpr, solution ) )
			{
				return false;
			}

			char solutionStr[MAX_EXPR_SIZE];
                        sprintf(solutionStr, "%d", solution);
			
			// Copy everything from the left of the open parenthesis into our new result
			char newExpr[ MAX_EXPR_SIZE ];
			strncpy( newExpr, result, openIdx );

			// Copy the solution to where the parentheses would have been
			int offset = openIdx;
			strncpy( newExpr + offset, solutionStr, strlen( solutionStr ) );

			// Copy everything from the right of the close parenthesis into our new result
			offset += strlen( solutionStr );
			int distToEnd = strlen( result ) - closeIdx - 1;
			strncpy( newExpr + offset, result + closeIdx + 1, distToEnd );

			// Add terminating character
			offset += distToEnd;
			newExpr[ offset ] = '\0';

			// Copy into result
			strcpy( result, newExpr );
		}
		else
		{
			parens = false;
		}
	}

	return true;
}

// UTILITY FUNCTIONS
// Returns true if character is a valid integer
bool isNumber( const char character )
{
	switch( character )
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return true;
	default:
		return false;
	}
}

// Returns true if character is a valid operator
bool isOperator( const char character )
{
	switch( character )
	{
	case '+':
	case '-':
	case '*':
	case '/':
		return true;
	default:
		return false;
	}
}
