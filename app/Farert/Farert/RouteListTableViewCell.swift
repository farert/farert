//
//  RouteListTableViewCell.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2015/06/28.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

class RouteListTableViewCell: UITableViewCell {
    
    @IBOutlet weak var routeString: UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }
    
    override func layoutSubviews() {
        if ((UIDevice.currentDevice().systemVersion as NSString).floatValue) < 8.0 {
            let bouds : CGRect = self.bounds
            self.routeString?.preferredMaxLayoutWidth = bounds.size.width - 12 - 12
        }
        super.layoutSubviews()
    }
    
    func heightForTitle(title : String) -> CGFloat {
        if 8.0 <= ((UIDevice.currentDevice().systemVersion as NSString).floatValue) {
            return UITableViewAutomaticDimension
        } else {
            self.routeString.text = title
            self.setNeedsLayout()
            self.layoutIfNeeded()
            let r : CGFloat = self.contentView.systemLayoutSizeFittingSize(UILayoutFittingCompressedSize).height
            if r < 44.0 {
                return 44.0
            } else {
                return r + 12.0
            }
        }
    }
}
