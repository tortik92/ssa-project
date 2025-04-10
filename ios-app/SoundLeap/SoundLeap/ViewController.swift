//
//  ViewController.swift
//  SoundLeap
//
//  Created by HTL_Developer on 16.03.25.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var digit6field: UITextField! // Text field for 6-digit input
    
    @IBOutlet weak var connectButton: UIButton! // Button to initiate connection, enabled when 6 digits entered

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
    }
}

// Extension to handle UITextFieldDelegate methods
extension ViewController: UITextFieldDelegate {

    // This method is called every time the user types or deletes a character in the text field
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
        
        // Get the current text in the field or use an empty string if nil
        let currentText = textField.text ?? ""
        
        // Update the text by applying the change the user just made
        let updatedText = (currentText as NSString).replacingCharacters(in: range, with: string)
        
        // Check if the updated text has more than 6 characters — if so, disallow the change
        if updatedText.count > 6 {
            return false
        }

        // Enable the connect button if exactly 6 characters are entered
        if(updatedText.count == 6) {
            connectButton.isEnabled = true;
        } else {
            // Disable the button if it's not 6 characters
            connectButton.isEnabled = false;
        }
        
        // Ensure only numerical digits are allowed
        let allowedCharacters = CharacterSet.decimalDigits
        let characterSet = CharacterSet(charactersIn: string)
        return allowedCharacters.isSuperset(of: characterSet)
    }
}
