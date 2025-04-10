//
//  ViewController.swift
//  SoundLeap
//
//  Created by HTL_Developer on 16.03.25.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var digit6field: UITextField!
    
    @IBOutlet weak var connectButton: UIButton!
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
    }
    
 



}


extension ViewController: UITextFieldDelegate {
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
        // Get the current text after adding the new characters
        let currentText = textField.text ?? ""
        let updatedText = (currentText as NSString).replacingCharacters(in: range, with: string)
        
        // Check if the updated text exceeds 6 characters
        if updatedText.count > 6 {
            return false
        }
        if(updatedText.count == 6) {
            connectButton.isEnabled = true;
        }
        else {
            connectButton.isEnabled = false;
        }
        
        // Optionally, ensure that only digits are allowed
        let allowedCharacters = CharacterSet.decimalDigits
        let characterSet = CharacterSet(charactersIn: string)
        return allowedCharacters.isSuperset(of: characterSet)
    }
}
