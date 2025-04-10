//
//  PreferenceTableCell.swift
//  SoundLeap
//
//  Created by HTL_Developer on 16.03.25.
//

import UIKit

protocol PreferenceTableViewCellDelegate: AnyObject {
    func updatePreference(preference: Preference)
}
class PreferenceTableViewCell: UITableViewCell {
    
    weak var delegate: PreferenceTableViewCellDelegate? // Delegate
    
    @IBOutlet weak var preferenceLabel: UILabel!
    @IBOutlet weak var valueLabel: UILabel!
    @IBOutlet weak var leftButton: UIButton!
    @IBOutlet weak var rightButton: UIButton!

    var onValueChanged: ((Any) -> Void)?
    var preference: Preference?

    override func awakeFromNib() {
        super.awakeFromNib()
        
        // Hide the buttons initially
        leftButton.isHidden = true
        rightButton.isHidden = true
    }
    
    // Configure the cell with preference data
    func configureCell(with preference: Preference) {
        self.preference = preference
        preferenceLabel.text = preference.preference_name
        
        // Show the relevant controls based on the preference type
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
    
    private func configureNumberCell(_ preference: Preference) {
        // Check if the default_value can be cast to Int
        if let defaultValue = preference.default_value as? Int {
            valueLabel.text = "\(defaultValue)"
            
            leftButton.isHidden = false
            rightButton.isHidden = false
            
            leftButton.addTarget(self, action: #selector(decrementNumber), for: .touchUpInside)
            rightButton.addTarget(self, action: #selector(incrementNumber), for: .touchUpInside)
        } else {
            // Handle the case where the default_value is not an Int
            valueLabel.text = "Invalid number"
        }
    }

    private func configureBoolCell(_ preference: Preference) {
        // Check if the default_value can be cast to Bool
        if let defaultValue = preference.default_value as? Bool {
            valueLabel.text = defaultValue ? "Yes" : "No"
            
            leftButton.isHidden = false
            rightButton.isHidden = false
            
            leftButton.setTitle("No", for: .normal)
            rightButton.setTitle("Yes", for: .normal)
            
            leftButton.addTarget(self, action: #selector(toggleBool), for: .touchUpInside)
            rightButton.addTarget(self, action: #selector(toggleBool), for: .touchUpInside)
        } else {
            // Handle the case where the default_value is not a Bool
            valueLabel.text = "Invalid boolean"
        }
    }

    private func configureListCell(_ preference: Preference) {
        // Check if the default_value can be cast to Int (index of the list)
        if let list = preference.list, let defaultValue = preference.default_value as? Int, defaultValue < list.count {
            valueLabel.text = list[defaultValue]
            
            leftButton.isHidden = false
            rightButton.isHidden = false
            
            leftButton.addTarget(self, action: #selector(decrementList), for: .touchUpInside)
            rightButton.addTarget(self, action: #selector(incrementList), for: .touchUpInside)
        } else {
            // Handle the case where the default_value is not valid
            valueLabel.text = "Invalid list index"
        }
    }

    // Handle number changes (increment/decrement)
    @objc private func decrementNumber() {
            if var preference = preference, let currentValue = preference.default_value as? Int {
                let newValue = max(currentValue - 1, preference.min_value ?? currentValue)
                preference.default_value = newValue
                delegate?.updatePreference(preference: preference)
                valueLabel.text = "\(newValue)"
                onValueChanged?(newValue)
            }
        }
        
        @objc private func incrementNumber() {
            if var preference = preference, let currentValue = preference.default_value as? Int {
                let newValue = min(currentValue + 1, preference.max_value ?? currentValue)
                preference.default_value = newValue
                delegate?.updatePreference(preference: preference)
                valueLabel.text = "\(newValue)"
                onValueChanged?(newValue)
            }
        }

    @objc private func toggleBool() {
        if var preference = preference, let currentValue = preference.default_value as? Bool {
            let newValue = !currentValue
            preference.default_value = newValue
            delegate?.updatePreference(preference: preference)
            valueLabel.text = newValue ? "Yes" : "No"
            onValueChanged?(newValue)
        }
    }

    @objc private func decrementList() {
        if var preference = preference, let list = preference.list, let currentValue = preference.default_value as? Int {
            let newValue = (currentValue - 1 + list.count) % list.count
            preference.default_value = newValue
            delegate?.updatePreference(preference: preference)
            valueLabel.text = list[newValue]
            onValueChanged?(newValue)
        }
    }

    @objc private func incrementList() {
        if var preference = preference, let list = preference.list, let currentValue = preference.default_value as? Int {
            let newValue = (currentValue + 1) % list.count
            preference.default_value = newValue
            delegate?.updatePreference(preference: preference)
            valueLabel.text = list[newValue]
            onValueChanged?(newValue)
        }
    }
}
