//
//  HeaderTableViewCell.swift
//  iFarert
//
//  Created by TAKEDA, Noriyuki on 2015/03/24.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

protocol TableHeaderViewDelegate: AnyObject {
    func tableHeaderViewTouched(_ cell: HeaderTableViewCell)
}

class HeaderTableViewCell: UITableViewCell {
    
    weak var delegate: TableHeaderViewDelegate? = nil

    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
        let tapGesture : UITapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(HeaderTableViewCell.selTapped(_:)))
        self.addGestureRecognizer(tapGesture)

    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

    @IBAction func selTapped(_ sender : AnyObject) {
        delegate?.tableHeaderViewTouched(self)
    }
}
