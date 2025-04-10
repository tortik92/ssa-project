//
//  PreferenceTableCell.swift
//  SoundLeap
//
//  Created by HTL_Developer on 16.03.25.
//

import UIKit

// MARK: - Protocol used for communication back to the controller
protocol PreferenceTableViewCellDelegate: AnyObject {
    func updatePreference(preference: Preference)
}

// MARK: - Custom UITableViewCell to handle user preference settings
class PreferenceTableViewCell: UITableViewCell {
    
    // Delegate for updating preference when changed
    weak var delegate: PreferenceTableViewCellDelegate?
    
    // Outlets connected from storyboard
    @IBOutlet weak var preferenceLabel: UILabel!
    @IBOutlet weak var valueLabel: UILabel!
    @IBOutlet weak var leftButton: UIButton!
    @IBOutlet weak var rightButton: UIButton!

    // Closure for optional external value updates
    var onValueChanged: ((Any) -> Void)?
    
    // Stored reference to current preference
    var preference: Preference?

    override func awakeFromNib() {
        super.awakeFromNib()
        
        // Hide control buttons by default; they'll show based on preference type
        leftButton.isHidden = true
        rightButton.isHidden = true
    }
    
    // MARK: - Main configuration method
    func configureCell(with preference: Preference) {
        self.preference = preference
        preferenceLabel.text = preference.preference_name
        
        // Decide which type of control to show based on preference type
        switch preference.preference_type {
        case "number":
            configureNumberCell(preference)
        case "bool":
            configureBoolCell(preference)
        case "list":
            configureListCell(preference)
        default:
            break
        }
    }

    // MARK: - Type-specific configuration methods
    
    // For numeric preferences (e.g., volume, count)
    private func configureNumberCell(_ preference: Preference) {
        if let defaultValue = preference.default_value as? Int {
            valueLabel.text = "\(defaultValue)"
            leftButton.isHidden = false
            rightButton.isHidden = false
            
            leftButton.addTarget(self, action: #selector(decrementNumber), for: .touchUpInside)
            rightButton.addTarget(self, action: #selector(incrementNumber), for: .touchUpInside)
        } else {
            valueLabel.text = "Invalid number"
        }
    }

    // For boolean preferences (e.g., sound on/off)
    private func configureBoolCell(_ preference: Preference) {
        if let defaultValue = preference.default_value as? Bool {
            valueLabel.text = defaultValue ? "Yes" : "No"
            
            leftButton.isHidden = false
            rightButton.isHidden = false
            
            leftButton.setTitle("No", for: .normal)
            rightButton.setTitle("Yes", for: .normal)
            
            leftButton.addTarget(self, action: #selector(toggleBool), for: .touchUpInside)
            rightButton.addTarget(self, action: #selector(toggleBool), for: .touchUpInside)
        } else {
            valueLabel.text = "Invalid boolean"
        }
    }

    // For list-based preferences (e.g., themes, languages)
    private func configureListCell(_ preference: Preference) {
        if let list = preference.list,
           let defaultValue = preference.default_value as? Int,
           defaultValue < list.count {
            
            valueLabel.text = list[defaultValue]
            leftButton.isHidden = false
            rightButton.isHidden = false
            
            leftButton.addTarget(self, action: #selector(decrementList), for: .touchUpInside)
            rightButton.addTarget(self, action: #selector(incrementList), for: .touchUpInside)
        } else {
            valueLabel.text = "Invalid list index"
        }
    }

    // MARK: - Action handlers for user input
    
    @objc private func decrementNumber() {
        if var preference = preference,
           let currentValue = preference.default_value as? Int {
            let newValue = max(currentValue - 1, preference.min_value ?? currentValue)
            preference.default_value = newValue
            delegate?.updatePreference(preference: preference)
            valueLabel.text = "\(newValue)"
            onValueChanged?(newValue)
        }
    }

    @objc private func incrementNumber() {
        if var preference = preference,
           let currentValue = preference.default_value as? Int {
            let newValue = min(currentValue + 1, preference.max_value ?? currentValue)
            preference.default_value = newValue
            delegate?.updatePreference(preference: preference)
            valueLabel.text = "\(newValue)"
            onValueChanged?(newValue)
        }
    }

    @objc private func toggleBool() {
        if var preference = preference,
           let currentValue = preference.default_value as? Bool {
            let newValue = !currentValue
            preference.default_value = newValue
            delegate?.updatePreference(preference: preference)
            valueLabel.text = newValue ? "Yes" : "No"
            onValueChanged?(newValue)
        }
    }

    @objc private func decrementList() {
        if var preference = preference,
           let list = preference.list,
           let currentValue = preference.default_value as? Int {
            let newValue = (currentValue - 1 + list.count) % list.count
            preference.default_value = newValue
            delegate?.updatePreference(preference: preference)
            valueLabel.text = list[newValue]
            onValueChanged?(newValue)
        }
    }

    @objc private func incrementList() {
        if var preference = preference,
           let list = preference.list,
           let currentValue = preference.default_value as? Int {
            let newValue = (currentValue + 1) % list.count
            preference.default_value = newValue
            delegate?.updatePreference(preference: preference)
            valueLabel.text = list[newValue]
            onValueChanged?(newValue)
        }
    }
}


