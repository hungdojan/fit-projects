/** FLP 2024
 *
 * This file contains a logic behind solving a 3x3x3 Rubik's cube using IDS. The tool 
 * starts by loading the initial cube state from the file follows by parsing and solving it.
 * Since this tool defines 18 types of possible moves it makes the state space enormous.
 * Therefor more complicated initial states will take longer (forever) to solve.
 * The initial depth is set to 4.
 *
 * This source file serves as a submission to the assigment of the course 
 * FLP at FIT, BUT 2024.
 *
 * Author: Hung Do (xdohun00)
 * Date: 27/04/2024
 * File: proj2.pl
*/


/**
 * read_line(-L, -C)
 * Reads input lines from the stdin until LF or EOF is reached.
 * This function was provided by the teachers in `input2.pl`.
 *
 * @param L     A list with a line content.
 * @param C     Last read character.
 */
read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),% atom_codes(C,[Cd]),
		[C|LL] = L).


/**
 * isEOFEOL(+C)
 * Checks if the given character is EOF or LF.
 * This function was provided by the teachers in `input2.pl`.
 *
 * @param C     A character to check.
 */
isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).


/**
 * read_lines(-Ls)
 * Construct each line from the stdin.
 * This function was provided by the teachers in `input2.pl`.
 *
 * @param Ls    A loaded list of input lines.
 */
read_lines(Ls) :-
	read_line(L,C),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).


/**
 * split_line(+First, -Second)
 * Splits a single lines by space characters to create a list of words.
 * This function was provided by the teachers in `input2.pl`.
 *
 * @param First     A line input.
 * @param Second    List of words.
 */
split_line([],[[]]) :- !.
split_line([' '|T], [[]|S1]) :- !, split_line(T,S1).
split_line([32|T], [[]|S1]) :- !, split_line(T,S1).    % aby to fungovalo i s retezcem na miste seznamu
split_line([H|T], [[H|G]|S1]) :- split_line(T,[G|S1]). % G je prvni seznam ze seznamu seznamu G|S1


/**
 * split_lines(+First, -Second)
 * Splits each lines into list of words.
 *
 * @param First     List of lines.
 * @param Second    List of words in the line.
 */
split_lines([],[]).
split_lines([L|Ls],[H|T]) :- split_lines(Ls,T), split_line(L,H).


/**
 * Program's starting point.
 */
start :-
    MaxDepth = 20,
    StartDepth = 2,
	prompt(_, ''),
	read_lines(LL),
	split_lines(LL,S),
    list_to_rubic(S, R),
    ( solve_cube(R, StartDepth, MaxDepth) ->
        print_moves
    ;
        writeln('Solution was not found!')
    ),
    halt.

:- dynamic pos/2.

/**
 * list_to_rubic(+First, -Second)
 * Parsing input data to create a Rubik's cube structure.
 *
 * @param First     List of loaded lines (expected 9 rows).
 * @param Second    Rubik's cube structure (list of 6 sides).
 */
list_to_rubic([[L51],
               [L52],
               [L53], 
               [L11, L21, L31, L41],
               [L12, L22, L32, L42],
               [L13, L23, L33, L43],
               [L61],
               [L62], 
               [L63]],
               [U, F, R, B, L, D]
) :- 
    create_side(L51, L52, L53, U),
    create_side(L11, L12, L13, F),
    create_side(L21, L22, L23, R),
    create_side(L31, L32, L33, B),
    create_side(L41, L42, L43, L),
    create_side(L61, L62, L63, D).

/**
 * create_side(+Top, +Middle, +Bottom, -Result)
 * Merges 3 lists into 1 list.
 *
 * @param Top       Top row.
 * @param Middle    Middle row.
 * @param Bottom    Bottom row.
 * @param Result    List of 9 pieces representing a side of the cube.
 */
create_side([T1, T2, T3],
            [M1, M2, M3],
            [B1, B2, B3],
            [T1, T2, T3, M1, M2, M3, B1, B2, B3]).

/**
 * side_is_solved(+Side)
 * Check if the given side is solved.
 *
 * @param Side 	Side of the cube.
 */
side_is_solved([X, X, X, X, X, X, X, X, X]).

/**
 * r_is_solved(+Cube)
 * Check if the given cube is solved.
 *
 * @param Cube	The cube structure.
 */
r_is_solved([U, F, R, B, L, D]) :-
    side_is_solved(U), 
    side_is_solved(F), 
    side_is_solved(R), 
    side_is_solved(B), 
    side_is_solved(L), 
    side_is_solved(D).


/**
 * solve_cube(+Pos, +CurrLimitDepth, +MaxDepth)
 * Main function that does the IDS algorithm. If the cube is still not solved after
 * finishing the deepening search iteration it increases the depth limitation and
 * rerun the function.
 *
 * @param Pos               Initial cube position.
 * @param CurrLimitDepth    Current iteration's depth.
 * @param MaxDepth          The maximum depth of the algorithm.
 */
solve_cube(Pos, _, _) :- r_is_solved(Pos), !.
solve_cube(Pos, CurrLimitDepth, MaxDepth) :-
    asserta(pos(Pos, 'Start')),
    CurrLimitDepth @=< MaxDepth,
    ( \+ iter_search(Pos, 0, CurrLimitDepth) ->
        NewDepth is CurrLimitDepth + 1,
        solve_cube(Pos, NewDepth, MaxDepth)
    ;
        true
    ).


/**
 * iter_search(+Pos, +Distance, +Limit)
 * Running one iteration of IDS algorithm.
 *
 * @param Pos		The current cube state.
 * @param Distance	Distance from the initial state.
 * @param Limit		The max depth limit.
 */
iter_search(Pos, _, _) :- r_is_solved(Pos), !.
iter_search(Pos, Distance, Limit) :-
    Distance @< Limit,
    NewDist is Distance + 1,
    r_move(M, Pos, NewPos),
    \+ pos(NewPos, _),
    assertz(pos(NewPos, M)),
    iter_search(NewPos, NewDist, Limit).
iter_search(Pos, _, _) :- retract(pos(Pos, _)), !, fail.


/**
 * Displays all the cube transitions from start to finish.
 */
print_moves :-
    findall(R, pos(R, _), [H|T]),
    print_cube(H),
    maplist(print_nl_cube, T). % handling new line prints

/**
 * print_nl_cube(+Item)
 * Prints new line before printing a cube.
 *
 * @param Item	The cube state to print.
 */
print_nl_cube(Item) :- nl, print_cube(Item).

/**
 * print_cube(+CubeState)
 * Cube displaying function.
 */
print_cube([U,
            [F1,F2,F3,F4,F5,F6,F7,F8,F9],
            [R1,R2,R3,R4,R5,R6,R7,R8,R9],
            [B1,B2,B3,B4,B5,B6,B7,B8,B9],
            [L1,L2,L3,L4,L5,L6,L7,L8,L9],
            D]) :-
    writef("%w%w%w\n%w%w%w\n%w%w%w\n", U),
    writef("%w%w%w %w%w%w %w%w%w %w%w%w\n", [F1,F2,F3, R1,R2,R3, B1,B2,B3, L1,L2,L3]),
    writef("%w%w%w %w%w%w %w%w%w %w%w%w\n", [F4,F5,F6, R4,R5,R6, B4,B5,B6, L4,L5,L6]),
    writef("%w%w%w %w%w%w %w%w%w %w%w%w\n", [F7,F8,F9, R7,R8,R9, B7,B8,B9, L7,L8,L9]),
    writef("%w%w%w\n%w%w%w\n%w%w%w\n", D).

/**
 * print_cube(+(Cube, Move))
 * Cube displaying function.
 */
print_cube((
            [U,
            [F1,F2,F3,F4,F5,F6,F7,F8,F9],
            [R1,R2,R3,R4,R5,R6,R7,R8,R9],
            [B1,B2,B3,B4,B5,B6,B7,B8,B9],
            [L1,L2,L3,L4,L5,L6,L7,L8,L9],
            D],
            Move
           )) :-
    write(Move), nl,
    writef("%w%w%w\n%w%w%w\n%w%w%w\n", U),
    writef("%w%w%w %w%w%w %w%w%w %w%w%w\n", [F1,F2,F3, R1,R2,R3, B1,B2,B3, L1,L2,L3]),
    writef("%w%w%w %w%w%w %w%w%w %w%w%w\n", [F4,F5,F6, R4,R5,R6, B4,B5,B6, L4,L5,L6]),
    writef("%w%w%w %w%w%w %w%w%w %w%w%w\n", [F7,F8,F9, R7,R8,R9, B7,B8,B9, L7,L8,L9]),
    writef("%w%w%w\n%w%w%w\n%w%w%w\n", D).
    

/** Defining each types of move. */
%! r_move(-Move, ?CurrState, ?NewState)
r_move(
    'U',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U31, U21, U11, U32, U22, U12, U33, U23, U13],
     [R11, R12, R13, F21, F22, F23, F31, F32, F33],
     [B11, B12, B13, R21, R22, R23, R31, R32, R33],
     [L11, L12, L13, B21, B22, B23, B31, B32, B33],
     [F11, F12, F13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]]
).

r_move(
    'U\'',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U13, U23, U33, U12, U22, U32, U11, U21, U31],
     [L11, L12, L13, F21, F22, F23, F31, F32, F33],
     [F11, F12, F13, R21, R22, R23, R31, R32, R33],
     [R11, R12, R13, B21, B22, B23, B31, B32, B33],
     [B11, B12, B13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]]
).

r_move(
    'D',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, L31, L32, L33],
     [R11, R12, R13, R21, R22, R23, F31, F32, F33],
     [B11, B12, B13, B21, B22, B23, R31, R32, R33],
     [L11, L12, L13, L21, L22, L23, B31, B32, B33],
     [D31, D21, D11, D32, D22, D12, D33, D23, D13]]
).

r_move(
    'D\'',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, R31, R32, R33],
     [R11, R12, R13, R21, R22, R23, B31, B32, B33],
     [B11, B12, B13, B21, B22, B23, L31, L32, L33],
     [L11, L12, L13, L21, L22, L23, F31, F32, F33],
     [D13, D23, D33, D12, D22, D32, D11, D21, D31]]
).

r_move(
    'L',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[B33, U12, U13, B23, U22, U23, B13, U32, U33],
     [U11, F12, F13, U21, F22, F23, U31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, D31, B21, B22, D21, B31, B32, D11],
     [L31, L21, L11, L32, L22, L12, L33, L23, L13],
     [F11, D12, D13, F21, D22, D23, F31, D32, D33]]
).

r_move(
    'L\'',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[F11, U12, U13, F21, U22, U23, F31, U32, U33],
     [D11, F12, F13, D21, F22, F23, D31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, U31, B21, B22, U21, B31, B32, U11],
     [L13, L23, L33, L12, L22, L32, L11, L21, L31],
     [B33, D12, D13, B23, D22, D23, B13, D32, D33]]
).

r_move(
    'R',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, F13, U21, U22, F23, U31, U32, F33],
     [F11, F12, D13, F21, F22, D23, F31, F32, D33],
     [R31, R21, R11, R32, R22, R12, R33, R23, R13],
     [U33, B12, B13, U23, B22, B23, U13, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, B31, D21, D22, B21, D31, D32, B11]]
).

r_move(
    'R\'',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, B31, U21, U22, B21, U31, U32, B11],
     [F11, F12, U13, F21, F22, U23, F31, F32, U33], 
     [R13, R23, R33, R12, R22, R32, R11, R21, R31],
     [D33, B12, B13, D23, B22, B23, D13, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, F13, D21, D22, F23, D31, D32, F33]]
).

r_move(
    'B',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[R13, R23, R33, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, D33, R21, R22, D32, R31, R32, D31],
     [B31, B21, B11, B32, B22, B12, B33, B23, B13],
     [U13, L12, L13, U12, L22, L23, U11, L32, L33],
     [D11, D12, D13, D21, D22, D23, L11, L21, L31]]
).

r_move(
    'B\'',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[L31, L21, L11, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, U11, R21, R22, U12, R31, R32, U13],
     [B13, B23, B33, B12, B22, B32, B11, B21, B31],
     [D31, L12, L13, D32, L22, L23, D33, L32, L33],
     [D11, D12, D13, D21, D22, D23, R33, R23, R13]]
).

r_move(
    'F',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, U13, U21, U22, U23, L33, L23, L13],
     [F31, F21, F11, F32, F22, F12, F33, F23, F13],
     [U31, R12, R13, U32, R22, R23, U33, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, D11, L21, L22, D12, L31, L32, D13],
     [R31, R21, R11, D21, D22, D23, D31, D32, D33]]
).

r_move(
    'F\'',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, U13, U21, U22, U23, R11, R21, R31],
     [F13, F23, F33, F12, F22, F32, F11, F21, F31],
     [D13, R12, R13, D12, R22, R23, D11, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, U33, L21, L22, U32, L31, L32, U31],
     [L13, L23, L33, D21, D22, D23, D31, D32, D33]]
).

r_move(
    'M',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, B32, U13, U21, B22, U23, U31, B12, U33],
     [F11, U12, F13, F21, U22, F23, F31, U32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, D32, B13, B21, D22, B23, B31, D12, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, F12, D13, D21, F22, D23, D31, F32, D33]]
).

r_move(
    'M\'',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, F12, U13, U21, F22, U23, U31, F32, U33],
     [F11, D12, F13, F21, D22, F23, F31, D32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, U32, B13, B21, U22, B23, B31, U12, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, B32, D13, D21, B22, D23, D31, B12, D33]]
).

r_move(
    'E',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, L21, L22, L23, F31, F32, F33],
     [R11, R12, R13, F21, F22, F23, R31, R32, R33],
     [B11, B12, B13, R21, R22, R23, B31, B32, B33],
     [L11, L12, L13, B21, B22, B23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]]
).

r_move(
    'E\'',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, R21, R22, R23, F31, F32, F33],
     [R11, R12, R13, B21, B22, B23, R31, R32, R33],
     [B11, B12, B13, L21, L22, L23, B31, B32, B33],
     [L11, L12, L13, F21, F22, F23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]]
).

r_move(
    'S',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, U13, L32, L22, L12, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, U21, R13, R21, U22, R23, R31, U23, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, D21, L13, L21, D22, L23, L31, D23, L33],
     [D11, D12, D13, R32, R22, R12, D31, D32, D33]]
).

r_move(
    'S\'',
    [[U11, U12, U13, U21, U22, U23, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, R12, R13, R21, R22, R23, R31, R32, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, L12, L13, L21, L22, L23, L31, L32, L33],
     [D11, D12, D13, D21, D22, D23, D31, D32, D33]],
    [[U11, U12, U13, R12, R22, R32, U31, U32, U33],
     [F11, F12, F13, F21, F22, F23, F31, F32, F33],
     [R11, D23, R13, R21, D22, R23, R31, D21, R33],
     [B11, B12, B13, B21, B22, B23, B31, B32, B33],
     [L11, U23, L13, L21, U22, L23, L31, U21, L33],
     [D11, D12, D13, L12, L22, L32, D31, D32, D33]]
).
