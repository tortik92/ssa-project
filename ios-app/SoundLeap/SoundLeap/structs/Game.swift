import Foundation

// Define the Preference struct to match the preference objects in the JSON
struct Preference {
    let preference_name: String
    let preference_type: String
    var default_value: CodableValue
    let min_value: Int?
    let max_value: Int?
    let button_preset: String
    let list: [String]?
    
    // CodableValue is used to handle different types for 'default_value' (can be number, bool, or string)
}
typealias CodableValue = Any
    
    
// Define the Game struct to match the main JSON structure
struct Game {
    let name: String
    let description: String
    let version: String
    let uid: String
    let preferences: [Preference]?
}

struct GameEasy: Codable {
    let name: String
    let description: String
    let version: String
    let uid: String
}

