//
//  GameSettingsViewController.swift
//  SoundLeap
//
//  Created by HTL_Developer on 16.03.25.
//

import UIKit

class GameSettingsViewController: UIViewController, UITableViewDataSource, UITableViewDelegate, PreferenceTableViewCellDelegate {

    // MARK: - Outlets from storyboard
    @IBOutlet weak var gameNamelabel: UILabel!
    @IBOutlet weak var tableView: UITableView!
    
    // MARK: - Properties
    var preferences: [Preference] = [] // Stores preference settings for the selected game
    var uid: String? // Unique identifier passed from the previous screen
    
    // MARK: - View Lifecycle
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Set up tableView delegates
        tableView.delegate = self
        tableView.dataSource = self
        
        // If UID is provided, fetch game data from server
        if let uid = uid {
            fetchGameData(uid: uid) { [weak self] game in
                guard let self = self else { return }
                if let game = game {
                    // Store preferences and reload table on main thread
                    self.preferences = game.preferences ?? []
                    DispatchQueue.main.async {
                        self.tableView.reloadData()
                    }
                } else {
                    print("Failed to fetch game data.")
                }
            }
        }
    }
    
    // MARK: - Fetching Game Data
    func fetchGameData(uid: String, completion: @escaping (Game?) -> Void) {
        guard let url = URL(string: "https://cakelab.co.nl/ssa-server?uid=\(uid)") else {
            print("Invalid URL")
            completion(nil)
            return
        }
        
        // Make the network request
        let task = URLSession.shared.dataTask(with: url) { data, response, error in
            if let error = error {
                print("Error fetching data: \(error.localizedDescription)")
                completion(nil)
                return
            }
            
            guard let data = data else {
                print("No data received")
                completion(nil)
                return
            }
            
            do {
                // Attempt to decode JSON manually
                if let json = try JSONSerialization.jsonObject(with: data, options: []) as? [String: Any] {
                    
                    // Extract main game properties
                    let name = json["name"] as? String ?? ""
                    let description = json["description"] as? String ?? ""
                    let version = json["version"] as? String ?? ""
                    let uid = json["uid"] as? String ?? ""
                    
                    // Parse preferences array
                    var preferences: [Preference] = []
                    if let preferenceArray = json["preferences"] as? [[String: Any]] {
                        for preferenceDict in preferenceArray {
                            // Extract individual preference attributes
                            let preferenceName = preferenceDict["preference_name"] as? String ?? ""
                            let preferenceType = preferenceDict["preference_type"] as? String ?? ""
                            let buttonPreset = preferenceDict["button_preset"] as? String ?? ""
                            let minValue = preferenceDict["min_value"] as? Int
                            let maxValue = preferenceDict["max_value"] as? Int
                            let list = preferenceDict["list"] as? [String]
                            let defaultValue: Any = preferenceDict["default_value"] ?? ""
                            
                            // Create and append Preference object
                            let preference = Preference(
                                preference_name: preferenceName,
                                preference_type: preferenceType,
                                default_value: defaultValue,
                                min_value: minValue,
                                max_value: maxValue,
                                button_preset: buttonPreset,
                                list: list
                            )
                            print(preference)
                            preferences.append(preference)
                        }
                    }
                    
                    // Create and return Game object
                    let game = Game(name: name, description: description, version: version, uid: uid, preferences: preferences)
                    completion(game)
                } else {
                    print("Invalid JSON format")
                    completion(nil)
                }
            } catch {
                print("Error parsing JSON: \(error.localizedDescription)")
                completion(nil)
            }
        }
        
        task.resume()
    }
    
    // MARK: - TableView DataSource
    
    // Return the number of preferences
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return preferences.count
    }
    
    // Configure and return each cell
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let cell = tableView.dequeueReusableCell(withIdentifier: "PreferenceCell", for: indexPath) as! PreferenceTableViewCell
        
        let preference = preferences[indexPath.row]
        cell.preferenceLabel.text = preference.preference_name
        
        // Display value depending on type
        if let boolValue = preference.default_value as? Bool {
            cell.valueLabel.text = boolValue ? "Yes" : "No"
        } else if let intValue = preference.default_value as? Int {
            cell.valueLabel.text = "\(intValue)"
        } else if let stringValue = preference.default_value as? String {
            cell.valueLabel.text = stringValue
        } else {
            cell.valueLabel.text = "Unknown Value"
        }
        
        // Configure cell with additional data and set delegate
        cell.configureCell(with: preference)
        cell.delegate = self
        return cell
    }
    
    // MARK: - PreferenceTableViewCellDelegate
    
    // Handle updates from cell delegate
    func updatePreference(preference: Preference) {
        if let index = preferences.firstIndex(where: { $0.preference_name == preference.preference_name }) {
            preferences[index] = preference // Update value
        }
        tableView.reloadData() // Refresh table
    }
    
    // MARK: - TableView Delegate
    
    // Handle row selection
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let preference = preferences[indexPath.row]
        print("Selected Preference: \(preference.preference_name)")
        
        // Future: Add logic for handling preference changes (e.g., present input UI)
    }
}

