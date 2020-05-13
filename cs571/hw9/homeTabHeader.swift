//
//  homeTabHeader.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/2/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class homeTabHeader: UICollectionReusableView {
    
    @IBOutlet weak var weatherView: UIView!
    
    @IBOutlet weak var cityLabel: UILabel!
    
    @IBOutlet weak var stateLabel: UILabel!
    
    @IBOutlet weak var temperatureLabel: UILabel!
    
    @IBOutlet weak var climateLabel: UILabel!
    
    @IBOutlet weak var weatherImage: UIImageView!
    
    
    override func awakeFromNib() {
        super.awakeFromNib()
        weatherImage.contentMode = .scaleAspectFill
        weatherImage.clipsToBounds = true
        weatherView.layer.cornerRadius = 10
        weatherView.layer.borderWidth = 1
        weatherView.layer.borderColor = UIColor.white.cgColor
        weatherView.clipsToBounds = true
    }
    
}
