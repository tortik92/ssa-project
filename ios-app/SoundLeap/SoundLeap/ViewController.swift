//
//  ViewController.swift
//  SoundLeap
//
//  Created by HTL_Developer on 16.03.25.
//

import UIKit
import CoreBluetooth

class MyPeripheral: NSObject {

        /// MARK: - BLE services and charcteristics Identifiers

        public static let mainUUID     = CBUUID.init(string: "0000ffe0-0000-1000-8000-00805f9b34fb")
        public static let secondary   = CBUUID.init(string: "0000ffe1-0000-1000-8000-00805f9b34fb")
}

// Extension to handle UITextFieldDelegate methods
extension ViewController: UITextFieldDelegate, UIViewController, CBCentralManagerDelegate, CBPeripheralDelegate {

    @IBOutlet weak var digit6field: UITextField! // Text field for 6-digit input
    
    @IBOutlet weak var connectButton: UIButton! // Button to initiate connection, enabled when 6 digits entered

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
    }

    let deviceUUID: [UInt8] = Array((UIDevice.current.identifierForVendor?.uuidString)!.utf8 ?? "ERR ".utf8)
    
    var centralManager: CBCentralManager!
    var myPeripheral: CBPeripheral!
    
    private var values: CBCharacteristic?
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == CBManagerState.poweredOn {
            print("BLE powered on")
            // Turned on
            central.scanForPeripherals(withServices: [MyPeripheral.mainUUID], options: nil)
        }
        else {
            print("Something wrong with BLE")
            // Not on, but can have different issues
        }
    }
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if let pname = peripheral.name {
            if pname == "SL-" + textField.text{
                print("Connected to: " + pname)
                self.centralManager.stopScan()
                
                self.myPeripheral = peripheral
                self.myPeripheral.delegate = self
                self.centralManager.connect(peripheral, options: nil)
            }
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected to " + peripheral.name!)
        self.myPeripheral.discoverServices([MyPeripheral.mainUUID])
    }
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print("Disconnected from " + peripheral.name!)
        central.scanForPeripherals(withServices: [MyPeripheral.mainUUID], options: nil)
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let services = peripheral.services {
            for service in services {
                print(service.uuid.uuidString, service.description)
                peripheral.discoverCharacteristics([MyPeripheral.mainUUID, MyPeripheral.secondary], for: service)
            }
        }
    }
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?)
    {
        if let characteristics = service.characteristics {
            for characteristic in characteristics {
                if(characteristic.uuid == MyPeripheral.secondary) {
                    values = characteristic
                }
            }
        }
    }
    
    private func writeData( characteristic: CBCharacteristic, value: Data, withNewLine: Bool) {
        let valuesToSend = Data(value)
        let separator: UInt8 = 0x7E
        var output: Data = Data(deviceUUID)
        output.append(separator)
        output.append(valuesToSend)
        
        if(withNewLine) {
            let newLine: [UInt8] = [0x0A,0x0D]
            output.append(Data(newLine))
        }
        
        // Check if it has the write property
        if characteristic.properties.contains(.writeWithoutResponse) && myPeripheral != nil {
            myPeripheral.writeValue(output, for: characteristic, type: .withoutResponse)
            
        }

    }

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
            centralManager = CBCentralManager(delegate: self, queue: nil)
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
