//
//  TrendingTab.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 4/21/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit
import Charts
import Alamofire
import SwiftyJSON

class TrendingTabViewController: UIViewController {

    
    @IBOutlet weak var searchInput: UITextField!
    
    @IBOutlet weak var trendingGraphView: LineChartView!
    
    var trendingData:[JSON] = [JSON]()
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        title = "Trending"
        configureTextField()
        configureTapCongesture()
        fetchGraphData(searchTerm: "Coronavirus")
    }
    
    
    private func configureTextField() {
        searchInput.delegate = self
        searchInput.returnKeyType = .done
    }
    
    
    private func configureTapCongesture(){
        let tapGesture = UITapGestureRecognizer(target: self, action: #selector(TrendingTabViewController.handleTap))
        view.addGestureRecognizer(tapGesture)
    }
    
    
    @objc func handleTap() {
        view.endEditing(true)
    }
    
    
    func fetchGraphData(searchTerm: String){
        var keyword = searchTerm.trimmingCharacters(in: .whitespacesAndNewlines)
        if keyword == "" {
            keyword = "Coronavirus"
        }
        let query:String = keyword.replacingOccurrences(of: " ", with: "%20")
        
        let url = "https://cs571-hw9-2270250947.wm.r.appspot.com/api/trends/?keyword=\(query)"
        
        Alamofire.request(url).responseJSON { (response) -> Void in
            if((response.result.value) != nil) {
                self.trendingData.removeAll()
                
                let json = JSON(response.result.value!)
                self.trendingData = json["data"].arrayValue
            }
            else{
                print("nil!")
            }
            self.updateGraph(searchTerm: keyword)
        }
        
    }
    
    
    func updateGraph(searchTerm: String) {
        
        var chartDataEntries = [ChartDataEntry]()
        for (index, value) in trendingData.enumerated() {
            chartDataEntries.append(ChartDataEntry(x: Double(index), y: value.doubleValue))
        }
        let trendingLine = LineChartDataSet(entries: chartDataEntries, label:"Trending Chart for \(searchTerm)")
        trendingLine.colors = [NSUIColor.link]
        trendingLine.setCircleColor(NSUIColor.link)
        trendingLine.circleHoleColor = UIColor.link
        trendingLine.circleRadius = 3.7
        let chartData = LineChartData()
        chartData.addDataSet(trendingLine)
        self.trendingGraphView.data = chartData
    }


}

extension TrendingTabViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        if textField == searchInput {
            textField.resignFirstResponder()
            fetchGraphData(searchTerm: searchInput.text!)
        }
        return true
    }
}
