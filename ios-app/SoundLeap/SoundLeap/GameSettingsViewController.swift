//
//  GameSettingsViewController.swift
//  SoundLeap
//
//  Created by HTL_Developer on 16.03.25.
//

import UIKit

class GameSettingsViewController: UIViewController,  UITableViewDataSource, UITableViewDelegate, PreferenceTableViewCellDelegate {
    
    
    
    @IBOutlet weak var gameNamelabel: UILabel!
    
    @IBOutlet weak var tableView: UITableView!
    
    var preferences: [Preference] = []
    var uid: String?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        tableView.delegate = self
        tableView.dataSource = self
        
        // Fetch the game data
        if let uid = uid {
            fetchGameData(uid: uid) { [weak self] game in
                guard let self = self else { return }
                if let game = game {
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
    
    func fetchGameData(uid: String, completion: @escaping (Game?) -> Void) {
        guard let url = URL(string: "https://cakelab.co.nl/ssa-server?uid=\(uid)") else {
            print("Invalid URL")
            completion(nil)
            return
        }
        
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
            
            // Attempt to parse data manually as a JSON dictionary
            do {
                if let json = try JSONSerialization.jsonObject(with: data, options: []) as? [String: Any] {
                    // Extract the game properties
                    let name = json["name"] as? String ?? ""
                    let description = json["description"] as? String ?? ""
                    let version = json["version"] as? String ?? ""
                    let uid = json["uid"] as? String ?? ""
                    
                    // Extract preferences
                    var preferences: [Preference] = []
                    if let preferenceArray = json["preferences"] as? [[String: Any]] {
                        for preferenceDict in preferenceArray {
                            let preferenceName = preferenceDict["preference_name"] as? String ?? ""
                            let preferenceType = preferenceDict["preference_type"] as? String ?? ""
                            let buttonPreset = preferenceDict["button_preset"] as? String ?? ""
                            let minValue = preferenceDict["min_value"] as? Int
                            let maxValue = preferenceDict["max_value"] as? Int
                            let list = preferenceDict["list"] as? [String]
                            
                            // Manually parse `default_value` using the flexible `Any` type
                            let defaultValue: Any = preferenceDict["default_value"] ?? ""
                            
                            // Create the Preference object
                            let preference = Preference(preference_name: preferenceName,
                                                        preference_type: preferenceType,
                                                        default_value: defaultValue,
                                                        min_value: minValue,
                                                        max_value: maxValue,
                                                        button_preset: buttonPreset,
                                                        list: list)
                            print(preference)
                            preferences.append(preference)
                        }
                    }
                    
                    // Create the Game object
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
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return preferences.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let cell = tableView.dequeueReusableCell(withIdentifier: "PreferenceCell", for: indexPath) as! PreferenceTableViewCell
        
        let preference = preferences[indexPath.row]
        cell.preferenceLabel.text = preference.preference_name
        
        // Safely unwrap and handle different types of default_value
        if let boolValue = preference.default_value as? Bool {
            cell.valueLabel.text = boolValue ? "Yes" : "No"
        } else if let intValue = preference.default_value as? Int {
            cell.valueLabel.text = "\(intValue)"
        } else if let stringValue = preference.default_value as? String {
            cell.valueLabel.text = stringValue
        } else {
            cell.valueLabel.text = "Unknown Value"
        }
        cell.configureCell(with: preference)
        cell.delegate = self
        return cell
    }
    
    func updatePreference(preference: Preference) {
            if let index = preferences.firstIndex(where: { $0.preference_name == preference.preference_name }) {
                preferences[index] = preference // Update the preference in the array
            }
            tableView.reloadData() // Reload table data to reflect changes
        }
    // MARK: - TableView Delegate
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let preference = preferences[indexPath.row]
        print("Selected Preference: \(preference.preference_name)")
        
        // Handle the change for the selected preference based on its type
    }}
