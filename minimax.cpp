#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include "gamestate.h"
#include "getmoves.h"
#include "minimax.h"
#define neg	-65536
#define pos	65536
using namespace std;

int flatwin(gamestate* game)
{
	int myflats = 0;
	int otherflats = 0;
	int n = game->n;
	for (int i=0; i<n; i++)
		for (int j=0; j<n; j++)
		{
			if (game->board[i][j][game->height[i][j]-1]==1||game->board[i][j][game->height[i][j]-1]==3)
				myflats++;
			if (game->board[i][j][game->height[i][j]-1]==-1||game->board[i][j][game->height[i][j]-1]==-3)
				otherflats++;
		}
	return (myflats-otherflats)*pos/(myflats+otherflats);
}

int eval(gamestate* game)
{
	int flatcount = 0; int f = 100; int cs = 90; int w = 30;
	int centre_control = 0; int c = 20; int ccap = 25;
	int stack_color = 0; int s = 25;
	int stack = 0;
	int influence = 0; int inf = 20; int total_influence = 0;
	int height;
	int i_ngb, j_ngb;
	int peice, opp_peice;
	int mycolor=0; int oppcolor=0;
	// vector<pair<int,int> > &v;

	int n = game->n;
	int** influence_table;
	influence_table = new int*[n];
	for (int i=0; i<n; i++)
	{
		influence_table[i] = new int[n];
		for (int j=0; j<n; j++)
			influence_table[i][j] = 0;
	}

	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			height = game->height[i][j];
			if (game->board[i][j][height-1]==1) //////// top flat mera
			{
				flatcount += f;
				if (i>=2 && i<=4 && j>=2 && j<=4)
					centre_control += c;
			}
			if (game->board[i][j][height-1]==-1) ////////// top flat uska
			{
				flatcount-=f;
				if (i>=2 && i<=4 && j>=2 && j<=4)
					centre_control -= c;
			}
			if (game->board[i][j][height-1]==3) //////// top cap mera
			{
				flatcount += cs;
				if (i>=2 && i<=4 && j>=2 && j<=4)
					centre_control += ccap;
			}
			if (game->board[i][j][height-1]==-3) ////////// top cap uska
			{
				flatcount -= cs;
				if (i>=2 && i<=4 && j>=2 && j<=4)
					centre_control -= ccap;
			}
			if (game->board[i][j][height-1]==2)
				flatcount += w;
			if (game->board[i][j][height-1]==-2)
				flatcount -= w;

			if (height > 1)
			{
				stack = 0;
				mycolor = 0;
				oppcolor = 0;
				for (int h=0; h<height-1; h++)
				{
					if (game->board[i][j][h] == 1)
						mycolor++;
					else
						oppcolor++;
				}
				stack = mycolor*s - oppcolor*s;
				peice = game->board[i][j][height-1];
				if (peice==1) stack += (stack>0)?2*s:s;
				else if (peice==-1) stack -= (stack>0)?0.5*s:2*s;
				else if (peice==3) stack += (stack>0)?s*5:3*s;
				else if (peice==-3) stack -= (stack>0)?s*3:s*5;
				else if (peice==2) stack += 3*s;
				else stack -= s*3;
				stack_color += stack;

				if (mycolor>=1 && peice > 0)
				{
					vector<pair<int,int> > v; game->getNeighbours(i,j,mycolor+1,v);
					for (int k=0; k<v.size(); k++)
					{
						i_ngb = v[k].first;
						j_ngb = v[k].second;
						opp_peice = game->board[i_ngb][j_ngb][game->height[i_ngb][j_ngb]-1];
						if (peice==1 || peice==2)
						{
							if (opp_peice!=-3 && opp_peice!=-2)
								influence_table[i_ngb][j_ngb]++;
						}
						else
						{
							if (opp_peice!=-3)
								influence_table[i_ngb][j_ngb]++;
						}
					}
				}
				if (oppcolor>=1 && peice < 0)
				{
					vector<pair<int,int> > v; game->getNeighbours(i,j,oppcolor+1,v);
					for (int k=0; k<v.size(); k++)
					{
						i_ngb = v[k].first;
						j_ngb = v[k].second;
						opp_peice = game->board[i_ngb][j_ngb][game->height[i_ngb][j_ngb]-1];
						if (peice==-1 || peice==-2)
						{
							if (opp_peice!=3 && opp_peice!=2)
								influence_table[i_ngb][j_ngb]--;
						}
						else
						{
							if (opp_peice!=3)
								influence_table[i_ngb][j_ngb]--;
						}
					}
				}

			}
			if (height==1)
			{
				peice = game->board[i][j][0];
				vector<pair<int,int> > v; game->getNeighbours(i,j,1,v);
				for (int k=0; k<v.size(); k++)
				{
					i_ngb = v[k].first;
					j_ngb = v[k].second;
					opp_peice = game->board[i_ngb][j_ngb][game->height[i_ngb][j_ngb]-1];
					if (peice==1 || peice==2)
					{
						if (opp_peice!=-3 && opp_peice!=-2)
							influence_table[i_ngb][j_ngb]++;
					}
					else if (peice==3)
					{
						if (opp_peice!=-3)
							influence_table[i_ngb][j_ngb]++;
					}
					if (peice==-1 || peice==-2)
					{
						if (opp_peice!=3 && opp_peice!=2)
							influence_table[i_ngb][j_ngb]--;	
					}
					else if (peice==-3)
					{
						if (opp_peice!=3)
							influence_table[i_ngb][j_ngb]--;
					}
				}
			}
		}
	}

	int row = 0; int col = 0;
	int row_influence = 0; int col_influence = 0;

	for (int i=0; i<n; i++)
	{
		row = 0;
		for (int j=0; j<n; j++)
		{
			total_influence += influence_table[i][j]*inf;
			row += influence_table[i][j];
		}
		if (row>=4*n/3)
			row_influence += 2*inf*row;
		if (row<=-4*n/3)
			row_influence -= 2*inf*row;
	}
	for (int j=0; j<n; j++)
	{
		col = 0;
		for (int i=0; i<n; i++)
		{
			col += influence_table[i][j];;
		}
		if (col>=4*n/3)
			col_influence += 2*inf*col;
		if (col<=-4*n/3)
			col_influence -= 2*inf*col;
	}

	return (flatcount + 0.2*centre_control + 0.2*stack_color + 0.6*total_influence + 0.5*row_influence + 0.5*col_influence);

}

int max_val(int a, int b)
{
	return (a>b)?a:b;
}

int min_val(int a, int b)
{
	return (a>b)?b:a;
}

bool myComparison(const pair<string,int> &a,const pair<string,int> &b)
{
	return a.second>b.second;
}
bool myComparison2(const pair<string,int> &a,const pair<string,int> &b)
{
	return a.second<b.second;
}

pair<int,string> value(gamestate* game, int cutoff, int depth, int alpha, int beta, bool maxNode)
{
	int game_over = game->over();

	if (game_over==1) ////// I won by road
		return make_pair(pos,"");

	if (game_over==-1) ////// Opp won by road
		return make_pair(neg,"");

	if (game_over==2) ////// Someone ran out of tiles
		return make_pair(flatwin(game),"");

	if (game_over==3) /////// Board full
		return make_pair(flatwin(game),"");

	if (cutoff==depth)
		return make_pair(eval(game),""); //// if it is maxnode =>
																						//// came from a move of minnode => evaluate wrt to other player	

	bool localW2F;

	if (maxNode)
	{
		// cerr<<"MAXNODE"<<endl<<endl;
		int max = neg;
		vector<string> moves; generate_moves(moves,game,game->player_id);
		int moves_size = moves.size();
		int max_index;
		int temp;

		vector<pair<string,int> > sorted_moves;
		for (int i=0; i<moves_size; i++)
		{
			game->update_board(moves[i],game->player_id);
			localW2F = game->wallToFlat;
			temp = eval(game);
			game->wallToFlat = localW2F;
			game->undo_move(moves[i],game->player_id);
			sorted_moves.push_back(make_pair(moves[i],temp));
		}
		sort(sorted_moves.begin(),sorted_moves.end(),myComparison);

		for (int i=0; i<moves_size; i++)
		{
			game->update_board(sorted_moves[i].first,game->player_id);
			// cerr<<"MAX MOVE"<<endl<<endl;
			// game->print_board();
			localW2F = game->wallToFlat;
			temp = value(game,cutoff,depth+1,alpha,beta,false).first;
			if (temp>max)
			{
				max = temp;
				max_index = i;
			}
			game->wallToFlat = localW2F;
			game->undo_move(sorted_moves[i].first,game->player_id);
			// cerr<<"UNDOING MAX MOVE"<<endl<<endl;
			// game->print_board();
			if (max >= beta)
				return make_pair(max,sorted_moves[i].first);
			alpha = max_val(alpha,max);
		}
		return make_pair(max,sorted_moves[max_index].first);
	}
	else
	{
		// cerr<<"MINNODE"<<endl<<endl;
		int min = pos;
		vector<string> moves; generate_moves(moves,game,game->other_player);
		int moves_size = moves.size();
		int min_index;
		int temp;

		vector<pair<string,int> > sorted_moves;
		for (int i=0; i<moves_size; i++)
		{
			game->update_board(moves[i],game->other_player);
			localW2F = game->wallToFlat;
			temp = eval(game);
			game->wallToFlat = localW2F;
			game->undo_move(moves[i],game->other_player);
			sorted_moves.push_back(make_pair(moves[i],temp));
		}
		sort(sorted_moves.begin(),sorted_moves.end(),myComparison2);

		for (int i=0; i<moves_size; i++)
		{
			game->update_board(sorted_moves[i].first,game->other_player);
			// cerr<<"MIN MOVE"<<endl<<endl;
			// game->print_board();
			localW2F = game->wallToFlat;
			temp = value(game,cutoff,depth+1,alpha,beta,true).first;
			if (temp<min)
			{
				min = temp;
				min_index = i;
			}
			game->wallToFlat = localW2F;
			game->undo_move(sorted_moves[i].first,game->other_player);
			// cerr<<"UNDOING MIN MOVE"<<endl<<endl;
			// game->print_board();
			if (min <= alpha) 
				return make_pair(min,sorted_moves[i].first);
			beta = min_val(beta,min);
		}
		return make_pair(min,sorted_moves[min_index].first);
	}
	
}