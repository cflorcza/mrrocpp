#include "mp/CubeState.h"

#include <stdio.h>

CubeState::CubeState ()
{
	up = UNKNOWN_CUBE_COLOR;
	down = UNKNOWN_CUBE_COLOR;
	front = UNKNOWN_CUBE_COLOR;
	rear = UNKNOWN_CUBE_COLOR;
	left = UNKNOWN_CUBE_COLOR;
	right = UNKNOWN_CUBE_COLOR;
};


CubeState::CubeState (CUBE_COLOR up_is, CUBE_COLOR down_is, CUBE_COLOR front_is, 
	CUBE_COLOR rear_is, CUBE_COLOR left_is, CUBE_COLOR right_is)
{
	set_state(up_is, down_is, front_is, rear_is, left_is, right_is);
};


CubeState::CubeState (const CubeState& cs)
{
	set_state(cs.up, cs.down, cs.front, cs.rear, cs.left, cs.right);
};


CubeState& CubeState::operator= (const CubeState& cs)
{
	if (this != &cs)
		this->set_state(cs.up, cs.down, cs.front, cs.rear, cs.left, cs.right);

	return *this;
};


void CubeState::set_state(CUBE_COLOR up_is, CUBE_COLOR down_is, CUBE_COLOR front_is, 
		CUBE_COLOR rear_is, CUBE_COLOR left_is, CUBE_COLOR right_is)
{
	up = up_is;
	down = down_is;
	front = front_is;
	rear = rear_is;
	left = left_is;
	right = right_is;
};


void CubeState::print_face_color(CUBE_COLOR face_name)
{
	switch (face_name)
	{
		case YELLOW:
			printf("YELLOW");
		break;
		case WHITE:
			printf("WHITE");
		break;
		case RED:
			printf("RED");
		break;
		case ORANGE:
			printf("ORANGE");
		break;
		case GREEN:
			printf("GREEN");
		break;
		case BLUE:
			printf("BLUE");
		break;
		default:
		break;			
	}	
};


void CubeState::print_cube_colors()
{
	printf("up: ");
	print_face_color(up);
	printf(", down: ");
	print_face_color(down);
	printf(", front: ");
	print_face_color(front);
	printf(", rear: ");
	print_face_color(rear);
	printf(", left: ");
	print_face_color(left);
	printf(", right: ");
	print_face_color(right);
	printf("\n");
};

CUBE_COLOR read_cube_color(char input_char)
{
	switch (input_char)
	{
		case 'Y':
			return YELLOW;
		break;
		case 'W':
			return WHITE;
		break;
		case 'R':
			return RED;
		break;
		case 'O':
			return ORANGE;
		break;
		case 'G':
			return GREEN;
		break;
		case 'B':
			return BLUE;
		break;
		default:
			return UNKNOWN_CUBE_COLOR;
		break;			
	}	
};
