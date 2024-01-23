//
//  ViewController.swift
//  MemoryGame
//
//  Created by Artem Panteleev on 16.09.23.
//  Last Update: 23.01.2024

import UIKit
import CoreBluetooth

class MyPeripheral: NSObject {

        /// MARK: - Particle LED services and charcteristics Identifiers

        public static let mainUUID     = CBUUID.init(string: "0000ffe0-0000-1000-8000-00805f9b34fb")
        public static let secondary   = CBUUID.init(string: "0000ffe1-0000-1000-8000-00805f9b34fb")

    }

class ViewController: UIViewController, CBCentralManagerDelegate, CBPeripheralDelegate {
    let deviceUUID: [UInt8] = Array((UIDevice.current.identifierForVendor?.uuidString)!.utf8 ?? "ERR ".utf8)
    
    var centralManager: CBCentralManager!
    var myPeripheral: CBPeripheral!
    
    private var values: CBCharacteristic?
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == CBManagerState.poweredOn {
            print("BLE powered on")
            statusLabel.text = "BLE poewered on"
            // Turned on
            central.scanForPeripherals(withServices: [MyPeripheral.mainUUID], options: nil)
        }
        else {
            print("Something wrong with BLE")
            statusLabel.text = "BLE Error"
            // Not on, but can have different issues
        }
    }
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if let pname = peripheral.name {
            if pname == "SL-5EC7" {
                print("Connected to: " + pname)
                statusLabel.text = "Found ["+pname+"]"
                self.centralManager.stopScan()
                
                self.myPeripheral = peripheral
                self.myPeripheral.delegate = self
                self.centralManager.connect(peripheral, options: nil)
            }
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected to " + peripheral.name!)
        statusLabel.text = "Connected to ["+peripheral.name!+"]"
        self.myPeripheral.discoverServices([MyPeripheral.mainUUID])
    }
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print("Disconnected from " + peripheral.name!)
        statusLabel.text = "Disconnected from ["+peripheral.name!+"]"
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
    


    @IBOutlet var collectionView: UICollectionView!
    @IBOutlet var statusLabel: UILabel!
    var colorData: [UIColor] = []
    
    @IBOutlet var startBtn: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        centralManager = CBCentralManager(delegate: self, queue: nil)
        statusLabel.text = "searching..."
        collectionView.dataSource = self
        collectionView.delegate = self
        
        if (colorData.isEmpty) {
            startBtn.isEnabled = false
        }
                
    }
    
    /**
        Function to add color of selected tile to wished to play.
        BackgroundColor of the Buttons needs to be the color, which will e send to pads
     
        - Parameters:
          - sender: Button, which was clicked
     
        - Returns:
          - void
    */
    @IBAction func buttonClick(_ sender: UIButton) {
        colorData.append(sender.backgroundColor ?? UIColor.red)
        collectionView.reloadData()
        
        startBtn.isEnabled = true
    }
    
    /**
        Function transaltes UIColors to Strings and sends them to pads via click
     
        - Parameters:
          - sender: Button, which was clicked to start the round
     
        - Returns:
          - void
    */
    @IBAction func startRound(_ sender: Any) {
    
        var colorDataUInt8: [UInt8] = []
        colorData.forEach{color in
            switch ((color.accessibilityName.components(separatedBy: " ").last ?? "red").lowercased()) {
                case "red":
                    colorDataUInt8.append(0x30)
                case "yellow":
                    colorDataUInt8.append(0x31)
                case "green":
                    colorDataUInt8.append(0x32)
                case "blue":
                    colorDataUInt8.append(0x33)
                default:
                    colorDataUInt8.append(0x00)
            }
        }
        colorData.removeAll()
        collectionView.reloadData()
        startBtn.isEnabled = false
        
        sendDataToPads(data: Data(colorDataUInt8))
    }
    
    /**
        Function sends daata to pads via Bluetooth
     
        - Parameters:
          - data: String with data included. Data to send to pads
     
        - Returns: void
    */
    func sendDataToPads(data: Data) {
        writeData(characteristic: values!, value: Data(data), withNewLine: true)
    }
    
}



extension ViewController: UICollectionViewDataSource {
    /**
        Function to specify the number of cells in collectionView
     
        - Parameters:
          - collectionView: Applied collectionView
          - section: Number of cells in applied collectionView
     
        - Returns:
          - void
    */
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
            return colorData.count
        }

    /**
        Function to change values and parameters of active cell
     
        - Parameters:
          - collectionView: Applied collectionView
          - indexPath: Applied cell
     
        - Returns: UICollectionViewCell: Modified cell
    */
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "cell", for: indexPath)
        cell.backgroundColor = colorData[indexPath.item]
        return cell
    }
    
    /// No idea
    /// - Parameters:
    ///   - collectionView: Applied collectionView
    ///   - kind: ???
    ///   - indexPath: Applied Cell
    /// - Returns:UICOllectionReusableView: ???
    func collectionView(_ collectionView: UICollectionView, viewForSupplementaryElementOfKind kind: String, at indexPath: IndexPath) -> UICollectionReusableView {
        let view = collectionView.dequeueReusableSupplementaryView(ofKind: UICollectionView.elementKindSectionHeader, withReuseIdentifier: "header", for: indexPath)
        return view
        }
}

extension ViewController: UICollectionViewDelegateFlowLayout {
    /// Function defined Layout for collectionViewCell
    /// - Parameters:
    ///   - collectionView: applied collectionView
    ///   - collectionViewLayout: existing Layout
    ///   - indexPath: index of applied cell
    /// - Returns: CGSize: Size and Layout of the cell
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {

        let width: CGFloat = 50
        let height: CGFloat = 50
        return CGSize(width: width, height: height)
    }
}

