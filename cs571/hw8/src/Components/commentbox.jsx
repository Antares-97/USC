import React, { Component } from 'react';
import commentBox from 'commentbox.io';

class CommentBox extends Component {
    constructor(props){
        super(props);
        this.state = {
        };
    }

    componentDidMount() {
        this.removeCommentBox = commentBox('5700532370931712-proj');
    }

    componentWillUnmount() {

        this.removeCommentBox();
    }

    render() {
        return (
            <div className="commentbox" id={this.props.id}/>
        );
    }
}
 
export default CommentBox;