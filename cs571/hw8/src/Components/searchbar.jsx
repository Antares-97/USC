import React, { Component } from 'react';
import AsyncSelect from 'react-select/async';
import { withRouter } from 'react-router-dom';
import '../App.css';

class SearchBar extends Component {
    constructor(props){
        super(props);
        this.state = {
            inputValue: '',
            suggestion: [],
            selectedOptions: [],
        }
        this.fetchOptions.bind(this);
        this.handleChange.bind(this);
        this.handleInputChange.bind(this);
        this.clearInputValue.bind(this);
    }

    componentDidMount(){
        this.props.handleClear(this.clearInputValue);
    }

    fetchOptions = (inputValue, callback) => {
        const url = "https://api.cognitive.microsoft.com/bing/v7.0/suggestions?q=" + inputValue;
        setTimeout( () => {
            fetch(url, {
                method: "GET",
                headers:{
                    'Ocp-Apim-Subscription-Key': 'd25921da6bf746a5a9d257eca0c390d8'
                }
            }).then((resp) => {
                return resp.json()
            }).then((data) => {
                let options = [];
                const result = data.suggestionGroups[0].searchSuggestions
                options = result.map(option => {
                    return {
                        label: option.query,
                        value: option.displayText
                    };
                });
                callback(options);
                this.setState({suggestion: options});
            })
            .catch((error) =>{
                console.log(error);
            });
        }, 100);
    }

    handleInputChange = (newValue) => {
        if(newValue){
            this.setState({inputValue: newValue});
        }
    }

    handleChange = (selectedOptions) => {
        this.setState({ selectedOptions });
        let url = '/results?keyword=' + selectedOptions.value;
        this.props.history.push(url);
    }

    clearInputValue = () => {
        this.setState({
            selectedOptions: [],
        });
    }

    render(){
        return (
            <div className="react-select-SearchBar">
                <AsyncSelect
                    value={this.state.selectedOptions}
                    loadOptions={this.fetchOptions}
                    placeholder="Enter Keyword .."
                    onChange={opt => this.handleChange(opt)}
                    defaultOptions={false}
                    onInputChange={this.handleInputChange}
                    
                />
            </div>
          );
    }
}
 
export default withRouter(SearchBar);