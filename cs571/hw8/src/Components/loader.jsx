import React from 'react';
import {css} from '@emotion/core';
import BounceLoader from 'react-spinners/BounceLoader';

function loader(props){
    const override = css`
        display: block;
        margin: 0 auto;
        border-color: red;
    `;

    return(
        <div className="central">
            <BounceLoader 
             css={override}
             color={"#2d4eca"}
             size={"30px"}
             loading={true}
            />

            Loading
        </div>
    );
}

export default loader;