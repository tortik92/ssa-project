//
//  ViewController.swift
//  MemoryGame
//
//  Created by Artem Panteleev on 16.09.23.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var collectionView: UICollectionView!
    
    var colorData: [UIColor] = []
    
    @IBOutlet weak var startBtn: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        collectionView.dataSource = self
        collectionView.delegate = self
        
        if (colorData.isEmpty) {
            startBtn.isEnabled = false
        }
                
    }
    
    
    @IBAction func buttonClick(_ sender: UIButton) {
        colorData.append(sender.backgroundColor ?? UIColor.red)
        collectionView.reloadData()
        
        startBtn.isEnabled = true
    }
    
    
    @IBAction func startRound(_ sender: Any) {
    var colorDataString: [String] = []
        colorData.forEach{color in
            colorDataString.append((color.accessibilityName.components(separatedBy: " ").last ?? "red").lowercased())
        }
        colorData.removeAll()
        collectionView.reloadData()
        print(colorDataString)
        sendDataToPads(array: colorDataString)
    }
    
    func sendDataToPads(array: [String]) {
        
    }
    
}



extension ViewController: UICollectionViewDataSource {
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
            return colorData.count
        }

        func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
            let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "cell", for: indexPath)
            cell.backgroundColor = colorData[indexPath.item]
            return cell
        }

        func collectionView(_ collectionView: UICollectionView, viewForSupplementaryElementOfKind kind: String, at indexPath: IndexPath) -> UICollectionReusableView {
            let view = collectionView.dequeueReusableSupplementaryView(ofKind: UICollectionView.elementKindSectionHeader, withReuseIdentifier: "header", for: indexPath)
            return view
        }
}

extension ViewController: UICollectionViewDelegateFlowLayout {
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {

        let width: CGFloat = 50
        let height: CGFloat = 50
        return CGSize(width: width, height: height)
    }
}

