//
//  FarertTableViewCell.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2015/04/18.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

class FarertTableViewCell: UITableViewCell {

    @IBOutlet weak var numberLabel: UILabel!
    @IBOutlet weak var numberContainer: UIView!
    @IBOutlet weak var lineLabel: UILabel!
    @IBOutlet weak var stationLabel: UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
        // デバッグ用（確認後に削除可能）
        print("🔍 UIView制約確認:")
        if let container = numberContainer {
            for constraint in container.constraints {
                print("📐 \(constraint)")
            }
        }
    }

    private func setupGradient() {
        print("🔍 setupGradient started")
        print("🔍 container bounds: \(numberContainer.bounds)")
        
        // 既存のグラデーションを削除
        numberContainer.layer.sublayers?.removeAll { $0 is CAGradientLayer }
        
        // サイズチェック
        guard numberContainer.bounds.width > 0 && numberContainer.bounds.height > 0 else {
            print("❌ Container size is zero!")
            return
        }
        
        let gradientLayer = CAGradientLayer()
        gradientLayer.frame = numberContainer.bounds
        
        gradientLayer.colors = [
            UIColor.purple.cgColor,
            UIColor.systemPurple.cgColor,
            UIColor.white.cgColor
        ]
        gradientLayer.locations = [0.0, 0.7, 1.0]  // 紫の領域を広く

        gradientLayer.startPoint = CGPoint(x: 0.5, y: 0)
        gradientLayer.endPoint = CGPoint(x: 1, y: 1)
        gradientLayer.cornerRadius = 9
        
        numberContainer.layer.insertSublayer(gradientLayer, at: 0)
        print("✅ Gradient layer added with frame: \(gradientLayer.frame)")
        print("✅ Total sublayers: \(numberContainer.layer.sublayers?.count ?? 0)")
    }
    
    
    func configure(number: Int, lineName: String, stationName: String) {
        numberLabel.text = "\(number)"
        lineLabel.text = lineName
        stationLabel.text = stationName
        
        numberContainer.backgroundColor = UIColor.clear
    }
    
    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

    override func layoutSubviews() {
        super.layoutSubviews()
        
        // レイアウト変更時にグラデーションを更新
        if numberContainer.bounds.width > 0 {
            setupGradient()
        }
    }
    
    override func prepareForReuse() {
        super.prepareForReuse()
        
        // セル再利用時のクリーンアップ
        numberLabel.text = ""
        lineLabel.text = ""
        stationLabel.text = ""
    }
}
