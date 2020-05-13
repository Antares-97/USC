//
//  BookmarksVC.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/7/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON
import Toast_Swift

private let reuseIdentifier = "Cell"

class BookmarksVC: UICollectionViewController {
    
    let defaults = UserDefaults.standard

    override func viewDidLoad() {
        super.viewDidLoad()
        
    }
    
    override func viewWillAppear(_ animated: Bool) {
        title = "Bookmarks"
//        clearUserDefaults()
        initData()
    }
    
    func clearUserDefaults() {
        if let bundleID = Bundle.main.bundleIdentifier {
            UserDefaults.standard.removePersistentDomain(forName: bundleID)
        }
    }
    
    func configureCollectionView() {
        self.collectionView.delegate = self
        self.collectionView.dataSource = self
    }
    
    @IBAction func cancelBookmark(_ sender: bookmarkButton) {
        initData()
        self.view.makeToast("Article removed from the Bookmarks", duration: 2.0, position: .bottom)
    }
    
    public func initData() {
        rawJSON = defaults.object(forKey: "bookmarks") as? [String] ?? [String]()
        newslist = [JSON]()
        if(rawJSON.count > 0){
            for rawstring in rawJSON{
                newslist.append(JSON.init(parseJSON: rawstring))
            }
        }
        DispatchQueue.main.async {
            self.collectionView.reloadData()
        }
    }
    
    func setEmptyMessage() {
        let messageLabel = UILabel()
        messageLabel.text = "No bookmarks added."
        messageLabel.textColor = .black
        messageLabel.numberOfLines = 0;
        messageLabel.textAlignment = .center;
        messageLabel.font = UIFont(name: "TrebuchetMS", size: 17)
        messageLabel.sizeToFit()
        messageLabel.center = self.view.center

        self.collectionView.backgroundView = messageLabel;
    }

    func restore() {
        self.collectionView.backgroundView = nil
    }
    
    private let reuseIdentifier = "bookmarksCell"
    private let itemsPerRow: CGFloat = 2
    private let sectionInsets = UIEdgeInsets(top: 5.0,
                                             left: 5.0,
                                             bottom: 5.0,
                                             right: 5.0)
    private var newslist = [JSON]()
    private var rawJSON = [String]()
    override func numberOfSections(in collectionView: UICollectionView) -> Int {
        return 1
    }


    override func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        if(newslist.count == 0){
            setEmptyMessage()
        }
        else {
            restore()
        }
        return newslist.count
    }

    override func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: reuseIdentifier, for: indexPath) as! NewsCollectionViewCell
        
        cell.titleLabel.text = newslist[indexPath.row]["title"].stringValue
        cell.sectionLabel.text = "| " + newslist[indexPath.row]["section"].stringValue
        cell.timeLabel.text = getDateFormat(originalDate: newslist[indexPath.row]["time"].stringValue)
        cell.bookmark = bookmarkButton(newsID: newslist[indexPath.row]["id"].stringValue)
        cell.news = newslist[indexPath.row]
    
        cell.backgroundColor = UIColor.lightGray
        cell.layer.borderColor = UIColor.gray.cgColor
        cell.layer.borderWidth = 1
        cell.layer.cornerRadius = 8
        cell.clipsToBounds = true

        if let url = newslist[indexPath.row]["image"].string {
            fetchImage(url: url, completionHandler: { image, _ in
                cell.newsImage.image = image
            })
        }
    
        return cell
    }
    
    override func collectionView(_ collectionView: UICollectionView, contextMenuConfigurationForItemAt indexPath: IndexPath, point: CGPoint) -> UIContextMenuConfiguration? {
        
        return UIContextMenuConfiguration(identifier: nil, previewProvider: nil, actionProvider: { suggestedActions in

            return self.makeContextMenu(for: self.newslist[indexPath.row])
        })
    }
    
    override func collectionView(_ collectionView: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        let detailedPageVC = UIStoryboard.init(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "collectionDetail") as! CollectionDetailVC
        detailedPageVC.news = newslist[indexPath.row]

        self.navigationController?.pushViewController(detailedPageVC, animated: true)
    }
    
    
    
    func getDateFormat(originalDate:String) -> String {
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "yyyy-MM-dd'T'HH:mm:ssZ"
        guard let publishDate = dateFormatter.date(from: originalDate) else { return "" }
        dateFormatter.dateFormat = "dd MMM"
        let formattedDate = dateFormatter.string(from: publishDate)
        return formattedDate
    }
    
    func fetchImage(url: String, completionHandler: @escaping (UIImage?, NetworkError) -> ()) {
        Alamofire.request(url).responseData { responseData in
            
            guard let imageData = responseData.data else {
                completionHandler(nil, .failure)
                return
            }
            
            guard let image = UIImage(data: imageData) else {
                completionHandler(nil, .failure)
                return
            }
            
            completionHandler(image, .success)
        }
    }
    
    func makeContextMenu(for news: JSON) -> UIMenu {
        
        let twitter = UIAction(title: "Share With Twitter", image: UIImage(named: "twitter")) { action in
            let tweetText = "Check out this Article!"
            let tweetUrl = news["url"].stringValue
            let tweetHashtag = "CSCI_571_NewsApp"
            let shareString = "https://twitter.com/intent/tweet?text=\(tweetText)&url=\(tweetUrl)&hashtags=\(tweetHashtag)"

            let escapedShareString = shareString.addingPercentEncoding(withAllowedCharacters: CharacterSet.urlQueryAllowed)!
            let url = URL(string: escapedShareString)
            UIApplication.shared.open(url!)
        }
        
        let defaults = UserDefaults.standard
        let flag = defaults.object(forKey: news["id"].stringValue) as? Bool ?? false
        let bookmarkTitle = flag ? "Remove Bookmark" : "Bookmark"
        let bookmarkImage = flag ? UIImage(systemName: "bookmark.fill") : UIImage(systemName: "bookmark")
        let bookmark = UIAction(title: bookmarkTitle, image: bookmarkImage) { action in
            let toastMessage = flag ? "Article removed from the Bookmarks" : "Article bookmarked. Check out the Bookmarks tab to view"
            self.view.makeToast(toastMessage, duration: 2.0, position: .bottom)
            defaults.set(!flag, forKey: news["id"].stringValue)
            self.initData()
        }
        
        return UIMenu(title: "Menu", children: [twitter, bookmark])
    }

}


extension BookmarksVC: UICollectionViewDelegateFlowLayout {
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {
        
        let paddingSpace = sectionInsets.left * (itemsPerRow + 1)
        let availableWidth = view.frame.width - paddingSpace
        let widthPerItem = availableWidth / itemsPerRow
        
        return CGSize(width: widthPerItem, height: 273.0)
    }
    
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, insetForSectionAt section: Int) -> UIEdgeInsets {
        return sectionInsets
    }
    
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, minimumInteritemSpacingForSectionAt section: Int) -> CGFloat {
        return sectionInsets.left
    }
    
}
