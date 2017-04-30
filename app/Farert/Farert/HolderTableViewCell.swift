//
//  HolderTableViewCell.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2017/05/02.
//  Copyright © 2017年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

class HolderTableViewCell: UITableViewCell {

    @IBOutlet weak var aggregateType: UIButton!
    @IBOutlet weak var holderItemTitle: UILabel!
    @IBOutlet weak var itemFare: UILabel!
    
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
