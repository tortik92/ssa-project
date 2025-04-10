//
//  GameSelectViewController.swift
//  SoundLeap
//
//  Created by HTL_Developer on 16.03.25.
//

import UIKit

class GameSelectViewController: UIViewController {
    
    
    @IBOutlet weak var tableView: UITableView!
    
    @IBOutlet weak var nextButton: UIButton!
    
    var games = [GameEasy]() // Array to hold fetched games
    var selectedGame: GameEasy? // Store selected game
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        tableView.delegate = self
        tableView.dataSource = self          // Fetch data from server
        fetchData { games in
            self.games = games
            self.tableView.reloadData()
        }
    }
    
    
    // Function to fetch data from server
    func fetchData(completion: @escaping ([GameEasy]) -> Void) {
        let urlString = "https://cakelab.co.nl/ssa-server" // Replace with actual URL
        guard let url = URL(string: urlString) else { return }
        
        let session = URLSession.shared
        let task = session.dataTask(with: url) { data, response, error in
            if let error = error {
                print("Error fetching data: \(error.localizedDescription)")
                return
            }
            
            if let data = data {
                do {
                    let decoder = JSONDecoder()
                    let games = try decoder.decode([GameEasy].self, from: data)
                    DispatchQueue.main.async {
                        completion(games)
                    }
                } catch {
                    print("Error decoding data: \(error.localizedDescription)")
                }
            }
        }
        task.resume()
    }
    
    @IBAction func nextButtonTapped(_ sender: UIButton) {
            // Make sure you have a valid selectedGame
            if let selectedGame = selectedGame {
                // Initialize the destination view controller
                if let gameDetailVC = storyboard?.instantiateViewController(withIdentifier: "GameSettingsViewController") as? GameSettingsViewController {
                    // Pass the selectedGame data to the destination view controller
                    gameDetailVC.uid = selectedGame.uid
                    
                    // Push the view controller onto the navigation stack
                    navigationController?.pushViewController(gameDetailVC, animated: true)
                }
            }
        }
    
}

// Table View Delegate and Data Source
extension GameSelectViewController: UITableViewDelegate, UITableViewDataSource {
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return games.count
    }
    
    // Dequeue the custom cell and use the labels
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "GameCell", for: indexPath) as! GameTableViewCell
        
        let game = games[indexPath.row]
        cell.titleLabel.text = game.name
        cell.descriptionLabel.text = game.description
        
        return cell
    }
    
    // Handle row selection
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        selectedGame = games[indexPath.row]
        nextButton.isEnabled = true // Enable the "Next" button when a row is selected
    }
}
