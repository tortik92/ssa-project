//
//  GameSelectViewController.swift
//  SoundLeap
//
//  Created by HTL_Developer on 16.03.25.
//

import UIKit

class GameSelectViewController: UIViewController {
    
    // Outlets connected from storyboard
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var nextButton: UIButton!
    
    // Array to hold the fetched games from the server
    var games = [GameEasy]()
    
    // Variable to keep track of the selected game
    var selectedGame: GameEasy?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Setting up the table view delegate and data source
        tableView.delegate = self
        tableView.dataSource = self
        
        // Fetch data from server and reload table view when done
        fetchData { games in
            self.games = games
            self.tableView.reloadData()
        }
    }
    
    // Function to fetch game data from the server
    func fetchData(completion: @escaping ([GameEasy]) -> Void) {
        let urlString = "https://cakelab.co.nl/ssa-server" // Replace with actual API endpoint
        guard let url = URL(string: urlString) else { return }
        
        let session = URLSession.shared
        let task = session.dataTask(with: url) { data, response, error in
            // Handle network error
            if let error = error {
                print("Error fetching data: \(error.localizedDescription)")
                return
            }
            
            // Handle received data
            if let data = data {
                do {
                    // Decode JSON response into array of GameEasy
                    let decoder = JSONDecoder()
                    let games = try decoder.decode([GameEasy].self, from: data)
                    
                    // Call completion on main thread to update UI
                    DispatchQueue.main.async {
                        completion(games)
                    }
                } catch {
                    // Handle JSON decoding error
                    print("Error decoding data: \(error.localizedDescription)")
                }
            }
        }
        task.resume() // Start the network request
    }
    
    // Action triggered when "Next" button is tapped
    @IBAction func nextButtonTapped(_ sender: UIButton) {
        // Proceed only if a game is selected
        if let selectedGame = selectedGame {
            // Instantiate the destination view controller
            if let gameDetailVC = storyboard?.instantiateViewController(withIdentifier: "GameSettingsViewController") as? GameSettingsViewController {
                // Pass the selected game's UID to the next screen
                gameDetailVC.uid = selectedGame.uid
                
                // Navigate to the game settings screen
                navigationController?.pushViewController(gameDetailVC, animated: true)
            }
        }
    }
}

// MARK: - Table View Delegate and Data Source Methods
extension GameSelectViewController: UITableViewDelegate, UITableViewDataSource {
    
    // Return number of rows based on games array count
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return games.count
    }
    
    // Configure and return the cell for a given row
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        // Dequeue reusable cell of type GameTableViewCell
        let cell = tableView.dequeueReusableCell(withIdentifier: "GameCell", for: indexPath) as! GameTableViewCell
        
        // Configure cell with game data
        let game = games[indexPath.row]
        cell.titleLabel.text = game.name
        cell.descriptionLabel.text = game.description
        
        return cell
    }
    
    // Handle what happens when a table row is selected
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        // Store the selected game
        selectedGame = games[indexPath.row]
        
        // Enable the "Next" button now that a selection is made
        nextButton.isEnabled = true
    }
}

