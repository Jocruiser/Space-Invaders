#include "game.h"

#include <iostream>
#include <string>
#include <vector>

#include "cpputils/graphics/image.h"

Game::Game() {
  background_.Initialize(800, 600);
  score_ = 0;
  lost_ = false;
}

Game::Game(int width, int height) { background_.Initialize(width, height); }

graphics::Image& Game::GetGameScreen() { return background_; }
Player& Game::GetPlayer() { return player_; }
int Game::GetScore() const { return score_; }
bool Game::HasLost() { return lost_; }

// getters for unique pointers
std::vector<std::unique_ptr<Opponent>>& Game::GetOpponents() {
  return opponents_;
}
std::vector<std::unique_ptr<PlayerProjectile>>& Game::GetPlayerProjectiles() {
  return player_projectiles_;
}
std::vector<std::unique_ptr<OpponentProjectile>>&
Game::GetOpponentProjectiles() {
  return opponent_projectiles_;
}


void Game::CreateOpponents() {
  //  Opponent opponent1(200, 5);
  //  Opponent opponent2(600, 5);
  int x = 400;
  int y = 200;

  int x_2 = 200;
  int y_2 = 200;
  std::unique_ptr<Opponent> opponent1 = std::make_unique<Opponent>(x, y);
  std::unique_ptr<Opponent> opponent2 = std::make_unique<Opponent>(x_2, y_2);
  opponents_.push_back(std::move(opponent1));
  opponents_.push_back(std::move(opponent2));
}
/*
void Game::CreateOpponentProjectiles() {
  // OpponentProjectile opponentproj1(200, 200);
  // OpponentProjectile opponentproj2(600, 200);
  int x = 500;
  int y = 300;

  int x_2 = 10;
  int y_2 = 5;
  std::unique_ptr<OpponentProjectile> opponentproj1 =
std::make_unique<OpponentProjectile>(x,y);
  std::unique_ptr<OpponentProjectile>
  opponentproj2 = std::make_unique<OpponentProjectile>(x_2,y_2);
  opponent_projectiles_.push_back(std::move(opponentproj1));
  opponent_projectiles_.push_back(std::move(opponentproj2));
}

void Game::CreatePlayerProjectiles() {
  // PlayerProjectile playerproj1(400, 400);
  // PlayerProjectile playerproj2(500, 400);
  int x = 200;
  int y = 100;

  int x_2 = 20;
  int y_2 = 20;
   std::unique_ptr<PlayerProjectile> playerproj1 =
  std::make_unique<PlayerProjectile>(x,y);
  std::unique_ptr<PlayerProjectile>
  playerproj2 = std::make_unique<PlayerProjectile>(x_2, y_2);
  player_projectiles_.push_back(std::move(playerproj1));
  player_projectiles_.push_back(std::move(playerproj2));
}
*/

void Game::Init() {
  CreateOpponents();
  player_.SetX(100);
  player_.SetY(100);
  // CreateOpponentProjectiles();
  // CreatePlayerProjectiles();

  background_.AddMouseEventListener(*this);
  background_.AddAnimationEventListener(*this);
}

/*void Game::RemoveInactive() {
  for (int i = opponents_.size() - 1; i >= 0; i++) {
   if (opponents_[i]->GetIsActive() == false) {
    opponents_.erase(opponents_.end() - (opponents_.size() - i));
  }
 }
}
*/
// graphics::Image deleted from parameter, compiled
void Game::UpdateScreen() {
  background_.DrawRectangle(0, 0, background_.GetWidth(),
                            background_.GetHeight(), 0, 0, 0);
  player_.Draw(background_);

  // draw text on screen to show user's score
std::string score_message = "SCORE: " + std::to_string(score_);
background_.DrawText(1, 1, score_message, 30, 255, 255, 255);

  for (int i = 0; i < player_projectiles_.size(); i++) {
    player_projectiles_[i]->Draw(background_);
  }

  for (int i = 0; i < opponent_projectiles_.size(); i++) {
    opponent_projectiles_[i]->Draw(background_);
  }

  for (int i = 0; i < opponents_.size(); i++) {
    opponents_[i]->Draw(background_);
  }
  if (HasLost()) {
  std::string game_over = "GAME OVER";
  background_.DrawText(400, 400, game_over, 30, 255, 255, 255);
  }
}
void Game::Start() { background_.ShowUntilClosed(); }

void Game::MoveGameElements() {
  for (int i = 0; i < player_projectiles_.size(); i++) {
    if (player_projectiles_[i]->GetIsActive()) {
      player_projectiles_[i]->Move(background_);
    }
  }
  for (int i = 0; i < opponent_projectiles_.size(); i++) {
    if (opponent_projectiles_[i]->GetIsActive()) {
      opponent_projectiles_[i]->Move(background_);
    }
  }
  for (int i = 0; i < player_projectiles_.size(); i++) {
    if (player_projectiles_[i]->GetIsActive()) {
      player_projectiles_[i]->Move(background_);
    }
  }
}

void Game::FilterIntersections() {
  for (int i = 0; i < opponents_.size(); i++) {
    if (opponents_[i]->IntersectsWith(&player_) &&
        opponents_[i]->GetIsActive() && player_.GetIsActive()) {
      player_.SetIsActive(false);
      opponents_[i]->SetIsActive(false);
    }
    for (int j = 0; j < player_projectiles_.size(); j++) {
      if (player_projectiles_[j]->IntersectsWith(opponents_[i].get()) &&
          player_projectiles_[j]->GetIsActive() &&
          opponents_[i]->GetIsActive()) {
        player_projectiles_[j]->SetIsActive(false);
        opponents_[i]->SetIsActive(false);
      }
    }

    for (int i = 0; i < opponent_projectiles_.size(); i++) {
      if (opponent_projectiles_[i]->IntersectsWith(&player_) &&
          opponent_projectiles_[i]->GetIsActive() && player_.GetIsActive()) {
        opponent_projectiles_[i]->SetIsActive(false);
        player_.SetIsActive(false);
      }
    }
  }
}

void Game::OnAnimationStep() {
  if (opponents_.size() == 0) {
    CreateOpponents();
  }
  MoveGameElements();
  LaunchProjectiles();
  FilterIntersections();
  RemoveInactive();
  UpdateScreen();
  background_.Flush();
}

void Game::OnMouseEvent(const graphics::MouseEvent& event) {
  if ((event.GetMouseAction() == graphics::MouseAction::kMoved ||
       event.GetMouseAction() == graphics::MouseAction::kDragged) &&
      (event.GetX() > 0 && event.GetX() < background_.GetWidth()) &&
      (event.GetY() > 0 && event.GetY() < background_.GetHeight())) {
    player_.SetX(event.GetX() - player_.GetWidth() / 2);
    player_.SetY(event.GetY() - player_.GetWidth() / 2);
  }
  if (event.GetMouseAction() == graphics::MouseAction::kDragged ||
      event.GetMouseAction() == graphics::MouseAction::kPressed) {
    std::unique_ptr<PlayerProjectile> player_projectiles_ptr =
        std::make_unique<PlayerProjectile>(event.GetX(), event.GetY());
    player_projectiles_.push_back(std::move(player_projectiles_ptr));
  }

  if (HasLost() && event.GetMouseAction() == graphics::MouseAction::kPressed) {
  Game Game1;
  Game1.Init();
  Game1.UpdateScreen();
  Game1.Start();
  }
}

void Game::LaunchProjectiles() {
 for (int i = 0; i < opponents_.size(); i++) {
 std::unique_ptr<OpponentProjectile> opponent_unique_ptr =
 opponents_[i]->LaunchProjectile();
 if (opponent_unique_ptr != nullptr) {
  opponent_projectiles_.push_back(std::move(opponent_unique_ptr));
  }
  }
  }


void Game::RemoveInactive() {
 for (int i = opponents_.size() - 1; i >= 0; i--) {
  if (opponents_[i]->GetIsActive() == false) {
   opponents_.erase(opponents_.end() - (opponents_.size() - i));
  }
 }
 
 for (int i = opponent_projectiles_.size() - 1; i >= 0; i--) {
  if ( opponent_projectiles_[i]->GetIsActive() == false) {
    opponent_projectiles_.erase( opponent_projectiles_.end() - ( opponent_projectiles_.size() - i));
  }
 }
 
 for (int i = player_projectiles_.size() - 1; i >= 0; i--) {
  if (player_projectiles_[i]->GetIsActive() == false) {
   player_projectiles_.erase(player_projectiles_.end() - (player_projectiles_.size() - i));
  }
 }
}
