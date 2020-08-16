/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2020 Cute Chess authors
    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "knightrelayboard.h"
#include "westernzobrist.h"

namespace Chess {

KnightRelayBoard::KnightRelayBoard()
        : WesternBoard(new WesternZobrist())
{
}

Board* KnightRelayBoard::copy() const
{
	return new KnightRelayBoard(*this);
}

QString KnightRelayBoard::variant() const
{
    return "knightrelay";
}

QString KnightRelayBoard::defaultFenString() const
{
    return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool KnightRelayBoard::hasEnPassantCaptures() const
{
    return false;
}

void KnightRelayBoard::generatePawnMoves(int sourceSquare, QVarLengthArray<Move> &moves) const
{
    WesternBoard::generateMovesForPiece(moves, Pawn, sourceSquare);

    Side side = sideToMove();
    bool knightRelay = false;

    for (int i = 0; i < m_knightOffsets.size(); i++)
    {
        Piece piece = pieceAt(sourceSquare + m_knightOffsets[i]);

        if (piece.type() == Knight && piece.side() == side)
        {
            knightRelay = true;
            break;
        }
    }

    if (knightRelay == true)
    {
        QVarLengthArray<int> pawnKnightRelayOffsets;
        pawnKnightRelayOffsets.clear();
        int targetRank;

        for (int i = 0; i< m_knightOffsets.size(); i++)
        {
            int targetSquare = sourceSquare + m_knightOffsets[i];
            targetRank = chessSquare(targetSquare).rank();
            if (targetRank != 0 || targetRank != height() - 1)
                pawnKnightRelayOffsets.append(m_knightOffsets[i]);
        }

        generateHoppingMoves(sourceSquare, pawnKnightRelayOffsets, moves);
    }
}

void KnightRelayBoard::generateMovesForPiece(QVarLengthArray<Move> &moves, int pieceType, int square) const
{
    if (pieceType == Pawn)
        generatePawnMoves(square, moves);
    if (pieceType != Pawn)
        WesternBoard::generateMovesForPiece(moves, pieceType, square);
}

void KnightRelayBoard::vInitialize()
{
    WesternBoard::vInitialize();

    int arwidth = width() + 2;

    m_knightOffsets.resize(8);
    m_knightOffsets[0] = -2 * arwidth - 1;
    m_knightOffsets[1] = -2 * arwidth + 1;
    m_knightOffsets[2] = -arwidth - 2;
    m_knightOffsets[3] = -arwidth + 2;
    m_knightOffsets[4] = arwidth - 2;
    m_knightOffsets[5] = arwidth + 2;
    m_knightOffsets[6] = 2 * arwidth - 1;
    m_knightOffsets[7] = 2 * arwidth + 1;
}

bool KnightRelayBoard::vIsLegalMove(const Move& move)
{
	if (captureType(move) == Knight)
		return false;

	// Pawns can promote normally, but not by knight-move relay.
	if (pieceAt(move.sourceSquare()).type() == Pawn
	&&  move.promotion() == Piece::NoPiece)
	{
		int targetRank = chessSquare(move.targetSquare()).rank();
		if (targetRank == 0 || targetRank == height() - 1)
			return false;
	}

    return WesternBoard::vIsLegalMove(move);
}

} // namespace Chess
