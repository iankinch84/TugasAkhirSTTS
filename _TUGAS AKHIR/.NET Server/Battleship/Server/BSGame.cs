using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Server {
  class BSGame {
    public static int _BOARD_SIZE = 8;
    public static int _BLANK = 0;
    public static int _OBJECT = 1;
    public static int _DESTROYED = 2;
    public static int _OPENED = 3;

    private int[][,] board = new int[2][,];

    private int _turn = 0;
    private List<String> Players = new List<String>();

    public String GameID { get; set; }
    public int Turn { get { return _turn; } }
    public Boolean CanAddPlayer { get { return (Players.Count < 2 ? true : false); } }
    public int PlayerCount { get { return Players.Count; } }

    private int maskBoard(int boardValue) {
      return (boardValue == _OBJECT ? _BLANK : boardValue);
    }

    public BSGame() {
      this.GameID = Guid.NewGuid().ToString();
      for(int p = 0; p < 2; p++) {
        board[p] = new int[_BOARD_SIZE, _BOARD_SIZE];
        for(int y = 0; y < _BOARD_SIZE; y++) { for(int x = 0; x < _BOARD_SIZE; x++) { board[p][y, x] = _BLANK; } }
      }
    }

    public void addPlayer(String newPlayer) {
      if(Players.Count < 2) { Players.Add(newPlayer); }
    }

    public int randomTurn() {
      Random rnd = new Random(DateTime.Now.Second * DateTime.Now.Millisecond);
      int currentTurn = rnd.Next(0, 2); _turn = currentTurn;
      return currentTurn;
    }
    public void changeTurn() {
      _turn = 1 - _turn;
    }

    public String playerIdForIndex(int playerIndex) {
      String id = "";
      if(playerIndex >= 0 && playerIndex <= 1) { id = Players[playerIndex]; }
      return id;
    }
    public int playerIndexForId(String playerId) {
      int index = -1;
      if(Players[0].ToUpper() == playerId.ToUpper()) { index = 0; } else if(Players[1].ToUpper() == playerId.ToUpper()) { index = 1; }
      return index;
    }

    public Boolean shoot(int playerIndex, int x, int y) {
      Boolean returnValue = false;
      if(playerIndex >= 0 && playerIndex < Players.Count) {
        int enemyIndex = 1 - playerIndex;
        int currentBoard = board[enemyIndex][y, x];

        if(currentBoard == _OBJECT) { currentBoard = _DESTROYED; returnValue = true; }
        if(currentBoard == _BLANK) { currentBoard = _OPENED; }

        board[enemyIndex][y, x] = currentBoard;
      }

      return returnValue;
    }
    public void setBoardForPlayer(int playerIndex, String newBoard) {
      if(playerIndex >= 0 && playerIndex < Players.Count && newBoard.Length == _BOARD_SIZE * _BOARD_SIZE) {
        for(int i = 0; i < newBoard.Length; i++) {
          board[playerIndex][i / _BOARD_SIZE, i % _BOARD_SIZE] = int.Parse(newBoard.Substring(i, 1));
        }
      }
    }
    public String boardForPlayer(int playerIndex, Boolean masked) {
      String rep = "";
      if(playerIndex >= 0 && playerIndex < Players.Count) {
        for(int y = 0; y < _BOARD_SIZE; y++) { for(int x = 0; x < _BOARD_SIZE; x++) { 
          rep += (masked == true ? maskBoard(board[playerIndex][y, x]).ToString() : board[playerIndex][y, x].ToString()); 
        } }
      }
      return rep;
    }
    public int lifeForPlayer(int playerIndex) {
      int life = 0;
      if(playerIndex >= 0 && playerIndex < Players.Count) {
        for(int y = 0; y < _BOARD_SIZE; y++) { for(int x = 0; x < _BOARD_SIZE; x++) { life += (board[playerIndex][y, x] == _OBJECT ? 1 : 0); } }
      }
      return life;
    }
    public void clearPlayer() {
      Players.Clear();
    }

  }
}
