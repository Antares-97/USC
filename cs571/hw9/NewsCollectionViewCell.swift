//
//  NewsCollectionViewCell.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/2/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit
import SwiftyJSON
import Toast_Swift


class NewsCollectionViewCell: UICollectionViewCell {
    
    
    @IBOutlet weak var newsImage: UIImageView!
    
    @IBOutlet weak var titleLabel: UILabel!
    
    @IBOutlet weak var sectionLabel: UILabel!
    
    @IBOutlet weak var timeLabel: UILabel!
    
    @IBOutlet var bookmark: bookmarkButton!
    
    var news = JSON()
    
    override func awakeFromNib() {
        super.awakeFromNib()
        newsImage.contentMode = .scaleAspectFill
        newsImage.clipsToBounds = true
    }
    
    func find(id: String, array: [String]) -> Int {
        for (index, rawstring) in array.enumerated()
        {
            let json = JSON.init(parseJSON: rawstring)
            if json["id"].stringValue == id {
                return index
            }
        }

        return -1
    }

    
    @IBAction func bookmarkBtn(_ sender: bookmarkButton) {
        let defaults = UserDefaults.standard
        let flag = defaults.object(forKey: news["id"].stringValue) as? Bool ?? false
        sender.isSelected = !flag
        
        var bookmarks = defaults.object(forKey: "bookmarks") as? [String] ?? [String]()
        let index = find(id: news["id"].stringValue, array: bookmarks)
        if(flag){        // remove
            if(index == -1){
                print("trying to delete an element which do not exist")
            }
            else{
                bookmarks.remove(at: index)
            }
        }
        else{           // add into bookmarks
            if(index == -1){
                bookmarks.append(news.rawString()!)
            }
            else{
                print("element already in the array")
            }
        }
        defaults.set(bookmarks, forKey: "bookmarks")
        defaults.set(!flag, forKey: news["id"].stringValue)
    }
    
}
