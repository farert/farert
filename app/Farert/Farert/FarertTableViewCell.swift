//
//  FarertTableViewCell.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2015/04/18.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

class FarertTableViewCell: UITableViewCell {

    @IBOutlet weak var lineName: UILabel!
    @IBOutlet weak var stationName: UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }
    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
