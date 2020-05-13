//
//  bookmarkButton.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/6/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit

class bookmarkButton: UIButton {
    
    let defaults = UserDefaults.standard
    
    required init(newsID: String) {
        super.init(frame: .zero)
//        initButton(newsID: newsID)
        
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
    }
    
    func initButton(newsID: String) {
        let flag = defaults.object(forKey: newsID) as? Bool ?? false
        self.isSelected = flag
        print(self.isSelected)
    }
    

}
