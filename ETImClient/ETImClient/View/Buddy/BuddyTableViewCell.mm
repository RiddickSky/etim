//
//  BuddyTableViewCell.m
//  ETImClient
//
//  Created by Ethan on 14/9/1.
//  Copyright (c) 2014年 Pingan. All rights reserved.
//

#import "BuddyTableViewCell.h"
#import "BuddyModel.h"


@interface BuddyTableViewCell () {
    UIImageView     *_iconImgView;
    UILabel         *_nameLabel;
    UILabel         *_signatureLabel;
}

@end


@implementation BuddyTableViewCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code
        _iconImgView = [[UIImageView alloc] initWithFrame:CGRectMake(12, 5, 40, 40)];
        _iconImgView.layer.cornerRadius = 4.0f;
        _iconImgView.layer.masksToBounds = YES;
        
        [self.contentView addSubview:_iconImgView];
        
        _nameLabel = [[UILabel alloc] initWithFrame:CGRectMake(RECT_MAX_X(_iconImgView) + 10, RECT_ORIGIN_Y(_iconImgView), 100, 20)];
        _nameLabel.font = BOLD_FONT(16);
        _nameLabel.textAlignment = NSTextAlignmentLeft;
        _nameLabel.backgroundColor = [UIColor clearColor];
        _nameLabel.textColor = [UIColor blackColor];
        [self.contentView addSubview:_nameLabel];
        
        _signatureLabel = [[UILabel alloc] initWithFrame:CGRectMake(RECT_ORIGIN_X(_nameLabel), RECT_MAX_Y(_nameLabel), RECT_WIDTH(self) - RECT_ORIGIN_X(_nameLabel), 25)];
        _signatureLabel.font = FONT(14);
        _signatureLabel.textColor = RGB_TO_UICOLOR(116, 116, 116);
        _signatureLabel.backgroundColor = [UIColor clearColor];
        [self.contentView addSubview:_signatureLabel];
    }
    return self;
}

- (void)update:(BuddyModel *)buddy {
    _iconImgView.image = [UIImage imageNamed:@"login_avatar_default"];
    _nameLabel.text = buddy.username;
    _signatureLabel.text = buddy.signature;
    
}
- (void)awakeFromNib
{
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end