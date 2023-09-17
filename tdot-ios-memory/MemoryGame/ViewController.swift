//
//  ViewController.swift
//  MemoryGame
//
//  Created by Artem Panteleev on 16.09.23.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet var collectionView: UICollectionView!
    
    var colorData: [UIColor] = []
    
    @IBOutlet var startBtn: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()

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
    var colorDataString: [String] = []
        colorData.forEach{color in
            colorDataString.append((color.accessibilityName.components(separatedBy: " ").last ?? "red").lowercased())
        }
        colorData.removeAll()
        collectionView.reloadData()
        startBtn.isEnabled = false
        
        sendDataToPads(data: String(colorDataString.joined(separator: ",")))
    }
    
    /**
        Function sends daata to pads via Bluetooth
     
        - Parameters:
          - data: String with data included. Data to send to pads
     
        - Returns: void
    */
    func sendDataToPads(data: String) {
        print(data)
        //TODO
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

